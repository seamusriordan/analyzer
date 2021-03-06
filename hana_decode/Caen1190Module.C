/** \class Caen1190 Module
    \author Stephen Wood
    \author Simona Malace
    \author Brad Sawatzky

Decoder module to retrieve Caen 1190 TDCs.  Based on CAEN 1190 decoding in
THaCodaDecoder.C in podd 1.5.   (Written by S. Malace, modified by B. Sawatzky)

*/

#include "Caen1190Module.h"
#include "THaSlotData.h"
#include <iostream>

using namespace std;

namespace Decoder {

  const Int_t NTDCCHAN = 128;
  const Int_t MAXHIT = 100;

  Module::TypeIter_t Caen1190Module::fgThisType =
    DoRegister( ModuleType( "Decoder::Caen1190Module" , 1190 ));

Caen1190Module::Caen1190Module(Int_t crate, Int_t slot) : VmeModule(crate, slot) {
  fDebugFile=0;
  Init();
}

Caen1190Module::~Caen1190Module() {
  if(fNumHits) delete [] fNumHits;
  if(fTdcData) delete [] fTdcData;
}

void Caen1190Module::Init() {
  Module::Init();
  fNumHits = new Int_t[NTDCCHAN];
  fTdcData = new Int_t[NTDCCHAN*MAXHIT];
  fDebugFile=0;
  Clear();
  IsInit = kTRUE;
  fName = "Caen TDC 1190 Module";
}

Int_t Caen1190Module::LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, const UInt_t *pstop) {
// This is a simple, default method for loading a slot
  const UInt_t *p = evbuffer;
  fWordsSeen = 0; 		// Word count including global header
  UInt_t evno;
  UInt_t slot=-1;
  UInt_t chan;
  UInt_t raw;
  Int_t status;
  Bool_t done=false;
  UInt_t chip_nr_hd;
  UInt_t flags;
  while( p <= pstop && !done) {
    fWordsSeen++;
    switch( (*p)&0xf8000000) {
    case 0xc0000000 :		// buffer alignment filler word; skip
      break;
    case 0x40000000 : 		// Global header
      evno=(*p & 0x07ffffe0) >> 5;
      slot=(*p & 0x0000001f);
      break;
    case 0x08000000 :  // chip header; contains: chip nr., ev. nr, bunch ID
      //chip_nr_hd = ((*p)&0x03000000) >> 24; // bits 25-24
      //bunch_id = ((*p)&0x00000fff); // bits 11-0
      break;
    case  0x00000000 : 		// measurement data
      chan=((*p)&0x03f80000)>>19; // bits 25-19
      raw=((*p)&0x0007ffff); // bits 18-0
      //      cout << slot << " " << chan << " " << raw << endl;
      status = sldat->loadData("tdc",chan,raw,raw);
      if(chan < NTDCCHAN) { 
	if(fNumHits[chan] < MAXHIT) {
	  fTdcData[chan*MAXHIT + fNumHits[chan]++] = raw;
	}
      }
      if (status != SD_OK ) return -1;
      break;
    case 0x18000000 : // chip trailer:  contains chip nr. & ev nr & word count
      // chip_nr_tr = ((*p)&0x03000000) >> 24; // bits 25-24
      /* If there is a chip trailer we assume there is a header too so we
       * cross check if chip nr stored in header matches chip nr. stored in
       * trailer.
       */
      // if (chip_nr_tr != chip_nr_hd) {
      // cerr  << "mismatch in chip nr between chip header and trailer"
      //      << " " << "header says: " << " " << chip_nr_hd
      //	      << " " << "trailer says: " << " " << chip_nr_tr << endl;
      //};
      //chip_nr_words = ((*p)&0x00000fff); // bits 11-0
      //nword_mod++;
      break;
    case 0x80000000 :  // global trailer: contains error status & word count per module & slot nr.
      //      status_err = ((*p)&0x07000000) >> 24; // bits 26-24
      //      if(status_err != 0) {
	//	cerr << "(evt: " << event_num << ", slot: "<< slot_from_gl_hd << ") ";
	//	cerr << "Error in 1190 status word: " << hex << status_err << dec << endl;
	//      }
      //      module_nr_words = ((*p)&0x001fffe0) >> 5; // bits 20-5
      //      slot_from_gl_tr = ((*p)&0x0000001f); // bits 4-0
      //      if (slot_from_gl_tr != slot_from_gl_hd) {
	//	cerr << "(evt: " << event_num << ", slot: "<< slot_from_gl_hd << ") ";
	//	cerr  << "mismatch in slot between global header and trailer"
	  //	      << " " << "header says: " << " " << slot_from_gl_hd
	  //	      << " " << "trailer says: " << " " << slot_from_gl_tr << endl;
	//      };
      //      nword_mod++;
      done = true;
      break;
    case 0x20000000:		// Output Buffer: TDC Error
      chip_nr_hd = ((*p)&0x03000000) >> 24; // bits 25-24
      flags = *p&0x7fff;			   // Error flags
      cout << "TDC1190 Error: Slot " << slot << ", Chip " << chip_nr_hd << 
	", Flags " << hex << flags << dec << " " << ", Ev #" << evno << endl;
      break;
    default:			// Unknown word
      cout << "unknown word for TDC1190: " << hex << (*p) << dec << endl;
      cout << "according to global header ev. nr. is: " << " " << evno << endl;
      break;
    }
    ++p;
  }
 
  return fWordsSeen;
}

Int_t Caen1190Module::GetData(Int_t chan, Int_t hit) const {
  if(hit >= fNumHits[chan]) return 0;
  Int_t idx = chan*MAXHIT + hit;
  if (idx < 0 || idx > MAXHIT*NTDCCHAN) return 0;
  return fTdcData[idx];
}

void Caen1190Module::Clear(const Option_t* opt) {
  VmeModule::Clear(opt);
  memset(fNumHits, 0, NTDCCHAN*sizeof(Int_t));
  memset(fTdcData, 0, NTDCCHAN*MAXHIT*sizeof(Int_t));
}

}

ClassImp(Decoder::Caen1190Module)
