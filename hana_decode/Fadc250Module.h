#ifndef Fadc250Module_
#define Fadc250Module_

/////////////////////////////////////////////////////////////////////
//
//   Fadc250Module
//   JLab FADC 250 Module
//
/////////////////////////////////////////////////////////////////////

#include "VmeModule.h"
#include "stdint.h"
#include <vector>

namespace Decoder {

  class Fadc250Module : public VmeModule {   // Inheritance

  public:
    
    Fadc250Module();                         // Default constructor
    Fadc250Module(Int_t crate, Int_t slot);  // Constructor
    virtual ~Fadc250Module();                // Virtual constructor

    using Module::GetData;
    using Module::LoadSlot;

    virtual void Clear(const Option_t *opt="");
    virtual void Init();
    virtual void CheckDecoderStatus() const;
    virtual Int_t GetPulseIntegralData(Int_t chan, Int_t ievent) const;
    virtual Int_t GetEmulatedPulseIntegralData(Int_t chan) const;
    virtual Int_t GetPulseTimeData(Int_t chan, Int_t ievent) const;
    virtual Int_t GetPulseCoarseTimeData(Int_t chan, Int_t ievent) const;
    virtual Int_t GetPulseFineTimeData(Int_t chan, Int_t ievent) const;
    virtual Int_t GetPulsePeakData(Int_t chan, Int_t ievent) const;
    virtual Int_t GetPulsePedestalData(Int_t chan, Int_t ievent) const;
    virtual Int_t GetPulseSamplesData(Int_t chan, Int_t ievent) const;
    virtual std::vector<uint32_t> GetPulseSamplesVector(Int_t chan) const;
    virtual Int_t GetFadcMode() const;
    virtual Int_t GetNumFadcEvents(Int_t chan) const;
    virtual Int_t GetNumFadcSamples(Int_t chan, Int_t ievent) const;
    virtual Int_t LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, const UInt_t *pstop);
    virtual Int_t LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, Int_t pos, Int_t len);
    virtual Int_t Decode(const UInt_t *pdat);
    virtual Bool_t IsMultiFunction();
    virtual Bool_t HasCapability(Decoder::EModuleType type);
            
  private:

    static const size_t NADCCHAN = 16;

    struct fadc_data_struct {
      // Header data objects
      uint32_t slot_blk_hdr, mod_id, iblock_num, nblock_events;     // Block header objects
      uint32_t PL, NSA, NSB;                                        // Block header objects cont.
      uint32_t slot_blk_trl, nwords_inblock;                        // Block trailer objects
      uint32_t slot_evt_hdr, evt_num;                               // Event header objects
      uint32_t trig_time;                                           // Trigger time objects
      // Window raw data objects
      uint32_t chan, win_width;                                     // FADC channel, window width
      uint32_t samples;                                             // FADC raw data samples
      bool overflow, invalid_samples;                               // True if any sample's "overflow" or "not valid" bit set, respectively
      // Pulse raw data objects
      uint32_t pulse_num, sample_num_tc;                            // FADC pulse number, sample number of threshold crossing
      // Pulse integral data objects
      uint32_t qual_factor, pulse_integral;                         // FADC quality factor, pulse integral
      // Pulse time data objects
      uint32_t coarse_pulse_time, fine_pulse_time, time;            // FADC pulse coarse time, pulse fine time, pulse time
      // Pulse pedestal data objects
      uint32_t pedestal, pulse_peak, pedestal_sum;                  // FADC pedestal, pulse peak value
      // Scaler data objects
      uint32_t scaler_words;                                        // FADC scaler words
      // Event block objects
      uint32_t evnt_of_blk;                                         // FADC event block information
      // Integral word objects
      uint32_t nsa_ext, sample_sum;                                 // FADC pulse parameters
      uint32_t samp_overflow, samp_underflow, samp_over_thresh;     // FADC pulse parameters
      // Time word object
      uint32_t peak_beyond_nsa, peak_not_found, peak_above_maxped;  // FADC pulse paramters
    } fadc_data;  // fadc_data_struct

    struct fadc_pulse_data {
      std::vector<uint32_t> integral, time, peak, pedestal;
      std::vector<uint32_t> samples, coarse_time, fine_time;
      void clear() {
	integral.clear(); time.clear(); peak.clear(); pedestal.clear();
	samples.clear(); coarse_time.clear(); fine_time.clear();
      }
    };
    std::vector<fadc_pulse_data> fPulseData; // Pulse data for each channel

    Bool_t data_type_4, data_type_6, data_type_7, data_type_8, data_type_9, data_type_10;
    Bool_t block_header_found, block_trailer_found, event_header_found, slots_match;
    uint32_t data_type_def;

    void ClearDataVectors();
    void PopulateDataVector(std::vector<uint32_t>& data_vector, uint32_t data);
    Int_t SumVectorElements(const std::vector<uint32_t>& data_vector) const;

    static TypeIter_t fgThisType;
    ClassDef(Fadc250Module,0)  //  JLab FADC 250 Module

  };  // Fadc250Module class

}  // Decoder namespace

#endif
