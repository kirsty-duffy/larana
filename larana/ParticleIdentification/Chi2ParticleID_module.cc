////////////////////////////////////////////////////////////////////////////
//
// \brief A chisq-test based particle identification method using calorimetry information
//
// \author tjyang@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#include "larana/ParticleIdentification/Chi2PIDAlg.h"
#include "lardata/Utilities/AssociationUtil.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "lardataobj/RecoBase/Track.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "fhiclcpp/ParameterSet.h"

namespace pid {
  class Chi2ParticleID;
}

class pid::Chi2ParticleID : public art::EDProducer {
public:
  explicit Chi2ParticleID(fhicl::ParameterSet const & p);

  virtual void produce(art::Event & e);

private:

  std::string fTrackModuleLabel;
  std::string fCalorimetryModuleLabel;

  Chi2PIDAlg fChiAlg;

};

pid::Chi2ParticleID::Chi2ParticleID(fhicl::ParameterSet const & p)
  : EDProducer{p}
  , fChiAlg(p.get< fhicl::ParameterSet >("Chi2PIDAlg"))
{
  fTrackModuleLabel = p.get< std::string >("TrackModuleLabel");
  fCalorimetryModuleLabel = p.get< std::string >("CalorimetryModuleLabel");

  produces< std::vector<anab::ParticleID>              >();
  produces< art::Assns<recob::Track, anab::ParticleID> >();
}

void pid::Chi2ParticleID::produce(art::Event & evt)
{
  art::Handle< std::vector<recob::Track> > trackListHandle;
  evt.getByLabel(fTrackModuleLabel,trackListHandle);

  std::vector<art::Ptr<recob::Track> > tracklist;
  art::fill_ptr_vector(tracklist, trackListHandle);

  art::FindManyP<anab::Calorimetry> fmcal(trackListHandle, evt, fCalorimetryModuleLabel);

  //if (!fmcal.isValid()) return;

  std::unique_ptr< std::vector<anab::ParticleID> > particleidcol(new std::vector<anab::ParticleID>);
  std::unique_ptr< art::Assns<recob::Track, anab::ParticleID> > assn(new art::Assns<recob::Track, anab::ParticleID>);

  if (fmcal.isValid()) {
  for (size_t trkIter = 0; trkIter < tracklist.size(); ++trkIter){   
    anab::ParticleID  pidout = fChiAlg.DoParticleID(fmcal.at(trkIter));
    particleidcol->push_back(pidout);
    util::CreateAssn(*this, evt, *particleidcol, tracklist[trkIter], *assn);
  }
  }
  evt.put(std::move(particleidcol));
  evt.put(std::move(assn));

  return;

}

////////////////////////////////////////////////////////////////////////
// Class:       Chi2ParticleID
// Module Type: producer
// File:        Chi2ParticleID_module.cc
//
// Generated at Fri Jul 13 16:06:02 2012 by Tingjun Yang using artmod
// from art v1_00_11.
////////////////////////////////////////////////////////////////////////


DEFINE_ART_MODULE(pid::Chi2ParticleID)
