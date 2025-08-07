/*
 * ntuple_JetInfo.cc
 *
 *  Created on: 13 Feb 2017
 *      Author: jkiesele
 */

#include "../interface/ntuple_JetInfo.h"
#include "../interface/helpers.h"
#include <vector>
#include <algorithm>
#include "DataFormats/Math/interface/deltaR.h"

using namespace std;
bool debug = false;
/// Simple object to store resonance and daughter GEN kinematics
class Resonance {
public:
  Resonance(){};
  ~Resonance(){};
  TLorentzVector p4;
  int daugdecay; // 1: tau_h tau_h; 2: tau_h tau_mu; 3: tau_h tau_e
  std::vector<int> daugid;
  std::vector<TLorentzVector> daugp4;
};

template<typename T> 
class PatPtSorter {
public:
  bool operator()(const T& i, const T& j) const {
    return (i.pt() > j.pt());
  }
};


template<typename T>
class PatRefPtSorter {
public:
  bool operator()(const T& i, const T& j) const {
    return (i->pt() > j->pt());
  }
};

void ntuple_JetInfo::getInput(const edm::ParameterSet& iConfig){

    gluonReduction_=(iConfig.getParameter<double>("gluonReduction"));
    jetPtMin_=(iConfig.getParameter<double>("jetPtMin"));
    jetPtMax_=(iConfig.getParameter<double>("jetPtMax"));
    jetAbsEtaMin_=(iConfig.getParameter<double>("jetAbsEtaMin"));
    jetAbsEtaMax_=(iConfig.getParameter<double>("jetAbsEtaMax"));
    min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
    SkipPU_ = (iConfig.getParameter<bool>("SkipPU"));
    KeepOnlyB_ = (iConfig.getParameter<bool>("KeepOnlyB"));
    KeepOnlyBC_ = (iConfig.getParameter<bool>("KeepOnlyBC"));
    KeepOnlyUDS_ = (iConfig.getParameter<bool>("KeepOnlyUDS"));
    KeepOnlyG_ = (iConfig.getParameter<bool>("KeepOnlyG"));
    KeepOnlyPU_ = (iConfig.getParameter<bool>("KeepOnlyPU"));
    KeepSingleTau_ = (iConfig.getParameter<bool>("KeepSingleTau"));  
    
    vector<string> disc_names = iConfig.getParameter<vector<string> >("bDiscriminators");
    for(auto& name : disc_names) {
        discriminators_[name] = 0.;
    }
}
void ntuple_JetInfo::initBranches(TTree* tree){

    //more general event info, here applied per jet
    addBranch(tree,"npv"    ,&npv_    ,"npv/F"    );
    addBranch(tree,"rho", &rho_, "rho/F");
    addBranch(tree,"ntrueInt",&ntrueInt_,"ntrueInt/F");
    addBranch(tree,"event_no"    ,&event_no_    ,"event_no/I"    );
    addBranch(tree,"jet_no"    ,&jet_no_    ,"jet_no/I"    );

    // truth labels
    addBranch(tree,"gen_pt"    ,&gen_pt_    ,"gen_pt_/F"    );
    addBranch(tree,"Delta_gen_pt"    ,&Delta_gen_pt_,"Delta_gen_pt_/F"    );

    addBranch(tree,"isMC",&isMC_, "isMC_/I");
    addBranch(tree,"isemu",&isemu_, "isemu_/I");
    addBranch(tree,"isdimu",&isdimu_, "isdimu_/I");
    addBranch(tree,"ismutau",&ismutau_, "ismutau_/I");
    
    addBranch(tree,"isB",&isB_, "isB_/I");
    addBranch(tree,"isGBB",&isGBB_, "isGBB_/I");
    addBranch(tree,"isBB",&isBB_, "isBB_/I");
    addBranch(tree,"isLeptonicB",&isLeptonicB_, "isLeptonicB_/I");
    addBranch(tree,"isLeptonicB_C",&isLeptonicB_C_, "isLeptonicB_C_/I");
    addBranch(tree,"isC",&isC_, "isC_/I");
    addBranch(tree,"isGCC",&isGCC_, "isGCC_/I");
    addBranch(tree,"isCC",&isCC_, "isCC_/I");
    addBranch(tree,"isD",&isD_, "isD_/I");
    addBranch(tree,"isU",&isU_, "isU_/I");
    addBranch(tree,"isS",&isS_, "isS_/I");
    addBranch(tree,"isG",&isG_, "isG_/I");
    addBranch(tree,"isMU",&isMU_, "isMU_/I");
    addBranch(tree,"isELE",&isELE_, "isELE_/I");
    addBranch(tree,"isTaup1h0p",&isTaup1h0p_, "isTaup1h0p_/I");
    addBranch(tree,"isTaup1h1p",&isTaup1h1p_, "isTaup1h1p_/I");
    addBranch(tree,"isTaup1h2p",&isTaup1h2p_, "isTaup1h2p_/I");
    addBranch(tree,"isTaup3h0p",&isTaup3h0p_, "isTaup3h0p_/I");
    addBranch(tree,"isTaup3h1p",&isTaup3h1p_, "isTaup3h1p_/I");
    addBranch(tree,"isTaupOther",&isTaupOther_, "isTaupOther_/I");
    addBranch(tree,"isTaum1h0p",&isTaum1h0p_, "isTaum1h0p_/I");
    addBranch(tree,"isTaum1h1p",&isTaum1h1p_, "isTaum1h1p_/I");
    addBranch(tree,"isTaum1h2p",&isTaum1h2p_, "isTaum1h2p_/I");
    addBranch(tree,"isTaum3h0p",&isTaum3h0p_, "isTaum3h0p_/I");
    addBranch(tree,"isTaum3h1p",&isTaum3h1p_, "isTaum3h1p_/I");
    addBranch(tree,"isTaumOther",&isTaumOther_, "isTaumOther_/I");
    addBranch(tree,"isDiTauh",&isDiTauh_, "isDiTauh_/I");
    addBranch(tree,"isDiTaumu",&isDiTaumu_, "isDiTaumu_/I");
    addBranch(tree,"isDiTaue",&isDiTaue_, "isDiTaue_/I");
    addBranch(tree,"isPU",&isPU_, "isPU_/I");
    addBranch(tree,"isUndefined",&isUndefined_, "isUndefined_/I");
    addBranch(tree,"genDecay",&genDecay_, "genDecay_/F"); 
    
    addBranch(tree,"jet_hflav", &jet_hflav_);
    addBranch(tree,"jet_pflav", &jet_pflav_);
    addBranch(tree,"jet_phflav", &jet_phflav_);

    // jet regression
    addBranch(tree,"jet_genmatch_pt", &jet_genmatch_pt_);
    addBranch(tree,"jet_genmatch_eta", &jet_genmatch_eta_);
    addBranch(tree,"jet_genmatch_phi", &jet_genmatch_phi_);
    addBranch(tree,"jet_genmatch_wnu_pt", &jet_genmatch_wnu_pt_);
    addBranch(tree,"jet_genmatch_wnu_eta", &jet_genmatch_wnu_eta_);
    addBranch(tree,"jet_genmatch_wnu_phi", &jet_genmatch_wnu_phi_);
    addBranch(tree,"jet_genmatch_lep_vis_pt", &jet_genmatch_lep_vis_pt_);
    addBranch(tree,"jet_mumatch_pt", &jet_mumatch_pt_);
    addBranch(tree,"jet_elematch_pt", &jet_elematch_pt_);
    addBranch(tree,"jet_taumatch_pt", &jet_taumatch_pt_);

    addBranch(tree,"skip_jet", &skip_jet_);

    // jet variables
    addBranch(tree,"jet_pt", &jet_pt_);
    addBranch(tree,"jet_corr_pt", &jet_corr_pt_);
    addBranch(tree,"jet_eta", &jet_eta_);
    addBranch(tree,"jet_phi", &jet_phi_);
    addBranch(tree,"jet_mass", &jet_mass_);
    addBranch(tree,"jet_energy", &jet_energy_);
    addBranch(tree,"jet_deepjet_probb", &jet_deepjet_probb_);

    //jet id
    addBranch(tree,"jet_looseId", &jet_looseId_);
    addBranch(tree,"jet_jetId", &jet_jetId_);
    addBranch(tree,"jet_puId", &jet_puId_); 

    // quark gluon
    addBranch(tree,"jet_qgl",   &jet_qgl_);  // qg tagger from jmar
    addBranch(tree,"QG_ptD",   &QG_ptD_);   // momentum fraction per jet constituent
    addBranch(tree,"QG_axis2", &QG_axis2_); // jet shape i.e. gluon are wider than quarks
    addBranch(tree,"QG_mult",  &QG_mult_);  // multiplicity i.e. total num of PFcands reconstructed

    addBranch(tree,"gen_pt_Recluster"    ,&gen_pt_Recluster_    ,"gen_pt_Recluster_/F"    );
    addBranch(tree,"gen_pt_WithNu"    ,&gen_pt_WithNu_    ,"gen_pt_WithNu_/F"    );
    addBranch(tree,"Delta_gen_pt_Recluster"    ,&Delta_gen_pt_Recluster_    ,"Delta_gen_pt_Recluster_/F"    );
    addBranch(tree,"Delta_gen_pt_WithNu"    ,&Delta_gen_pt_WithNu_    ,"Delta_gen_pt_WithNu_/F"    );

    // Ele/Mu/Tau gen
    addBranch(tree,"gen_number", &gen_number_, "gen_number_/I");
    addBranch(tree,"gend_number", &gend_number_, "gend_number_/I");
    addBranch(tree,"gen_particle_pt", &gen_particle_pt_, "gen_particle_pt_[gen_number_]/F");
    addBranch(tree,"gen_particle_eta", &gen_particle_eta_, "gen_particle_eta_[gen_number_]/F");
    addBranch(tree,"gen_particle_phi", &gen_particle_phi_, "gen_particle_phi_[gen_number_]/F");
    addBranch(tree,"gen_particle_mass", &gen_particle_mass_, "gen_particle_mass_[gen_number_]/F");
    addBranch(tree,"gen_particle_id", &gen_particle_id_, "gen_particle_id_[gen_number_]/F");
    addBranch(tree,"gen_particle_status", &gen_particle_status_, "gen_particle_status_[gen_number_]/F");
    addBranch(tree,"gen_particle_daughters_id", &gen_particle_daughters_id_, "gen_particle_daughters_id_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_igen", &gen_particle_daughters_igen_, "gen_particle_daughters_igen_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_pt", &gen_particle_daughters_pt_, "gen_particle_daughters_pt_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_eta", &gen_particle_daughters_eta_, "gen_particle_daughters_eta_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_phi", &gen_particle_daughters_phi_, "gen_particle_daughters_phi_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_mass", &gen_particle_daughters_mass_, "gen_particle_daughters_mass_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_status", &gen_particle_daughters_status_, "gen_particle_daughters_status_[gend_number_]/F");
    addBranch(tree,"gen_particle_daughters_charge", &gen_particle_daughters_charge_, "gen_particle_daughters_charge_[gend_number_]/F");
    addBranch(tree,"nGenBmeson", &nGenBmeson_);
    addBranch(tree,"genBmeson_daughter_pt", &genBmeson_daughter_pt_);
    addBranch(tree,"genBmeson_daughter_eta", &genBmeson_daughter_eta_);
    addBranch(tree,"genBmeson_daughter_phi", &genBmeson_daughter_phi_);
    addBranch(tree,"genBmeson_daughter_pdgId", &genBmeson_daughter_pdgId_);
    addBranch(tree,"genBmeson_daughter_mesonIdx", &genBmeson_daughter_mesonIdx_);
    addBranch(tree,"genBmeson_pt", &genBmeson_pt_);
    addBranch(tree,"genBmeson_eta", &genBmeson_daughter_eta_);
    addBranch(tree,"genBmeson_phi", &genBmeson_daughter_phi_);
    addBranch(tree,"nGenCmeson", &nGenCmeson_);
    addBranch(tree,"genCmeson_daughter_pt", &genCmeson_daughter_pt_);
    addBranch(tree,"genCmeson_daughter_eta", &genCmeson_daughter_eta_);
    addBranch(tree,"genCmeson_daughter_phi", &genCmeson_daughter_phi_);
    addBranch(tree,"genCmeson_daughter_pdgId", &genCmeson_daughter_pdgId_);
    addBranch(tree,"genCmeson_daughter_mesonIdx", &genCmeson_daughter_mesonIdx_);
    addBranch(tree,"genCmeson_pt", &genCmeson_pt_);
    addBranch(tree,"genCmeson_eta", &genCmeson_daughter_eta_);
    addBranch(tree,"genCmeson_phi", &genCmeson_daughter_phi_);
    addBranch(tree,"nConstituent", &nConstituent_);
    addBranch(tree,"constituent_pt", &constituent_pt_);
    addBranch(tree,"constituent_eta", &constituent_eta_);
    addBranch(tree,"constituent_phi", &constituent_phi_);
    addBranch(tree,"constituent_pdgId", &constituent_pdgId_);
    addBranch(tree,"constituent_BdaughterDr", &constituent_BdaughterDr_);
    addBranch(tree,"constituent_BdaughterIdx",  &constituent_BdaughterIdx_);
    addBranch(tree,"constituent_CdaughterDr", &constituent_CdaughterDr_);
    addBranch(tree,"constituent_CdaughterIdx",  &constituent_CdaughterIdx_);
    addBranch(tree,"nMatchedDaughtersB", &nMatchedDaughtersB_);
    addBranch(tree,"nMatchedDaughtersC", &nMatchedDaughtersC_);
    addBranch(tree,"isMatchedB", &isMatchedB_);
    addBranch(tree,"isMatchedBB", &isMatchedBB_);
    addBranch(tree,"isMatchedC", &isMatchedC_);
    addBranch(tree,"isMatchedBAndC", &isMatchedBAndC_);
    addBranch(tree,"isNotMatched", &isNotMatched_);
    addBranch(tree,"isMatchedB1d", &isMatchedB1d_);
    addBranch(tree,"isMatchedB2d", &isMatchedB2d_);
    addBranch(tree,"isMatchedB3d", &isMatchedB3d_);
    addBranch(tree,"isMatchedBMore3d", &isMatchedBMore3d_);
    addBranch(tree,"isMatchedBB2d", &isMatchedBB2d_);
    addBranch(tree,"isMatchedBB3d", &isMatchedBB3d_);
    addBranch(tree,"isMatchedBBMore4d", &isMatchedBBMore4d_);
    addBranch(tree,"isMatchedC1d", &isMatchedC1d_);
    addBranch(tree,"isMatchedC2d", &isMatchedC2d_);
    addBranch(tree,"isMatchedC3d", &isMatchedC3d_);
    addBranch(tree,"isMatchedCMore3d", &isMatchedCMore3d_);
    addBranch(tree,"isMatchedB1dC1d", &isMatchedB1dC1d_);
    addBranch(tree,"isMatchedB2dC1d", &isMatchedB2dC1d_);
    addBranch(tree,"isMatchedBMore2dC1d", &isMatchedBMore2dC1d_);
    addBranch(tree,"isMatchedB1dC2d", &isMatchedB1dC2d_);
    addBranch(tree,"isMatchedB2dC2d", &isMatchedB2dC2d_);
    addBranch(tree,"isMatchedBMore2dC2d", &isMatchedBMore2dC2d_);
    addBranch(tree,"isMatchedB1dCMore2d", &isMatchedB1dCMore2d_);
    addBranch(tree,"isMatchedB2dCMore2d", &isMatchedB2dCMore2d_);
    addBranch(tree,"isMatchedBMore2dCMore2d", &isMatchedBMore2dCMore2d_);



    /*if(1) // discriminators might need to be filled differently. FIXME
        for(auto& entry : discriminators_) {
            string better_name(entry.first);
            std::replace(better_name.begin(), better_name.end(), ':', '_');
            addBranch(tree,better_name.c_str(), &entry.second, (better_name+"/F").c_str());
        }*/
}

void ntuple_JetInfo::readEvent(const edm::Event& iEvent){

    iEvent.getByToken(qglToken_, qglHandle);
    iEvent.getByToken(ptDToken_, ptDHandle);
    iEvent.getByToken(axis2Token_, axis2Handle);
    iEvent.getByToken(multToken_, multHandle);

    iEvent.getByToken(genJetMatchReclusterToken_, genJetMatchRecluster);
    iEvent.getByToken(genJetMatchWithNuToken_, genJetMatchWithNu);
    iEvent.getByToken(genJetMatchAllowDuplicatesToken_, genJetMatchAllowDuplicates);

    iEvent.getByToken(genParticlesToken_, genParticlesHandle);
    iEvent.getByToken(genJetsWnuToken_, genJetsWnuH);
    iEvent.getByToken(genJetsToken_, genJetsH);

    iEvent.getByToken(muonsToken_, muonsHandle);
    iEvent.getByToken(electronsToken_, electronsHandle);
    iEvent.getByToken(tausToken_,tausH);

    event_no_=iEvent.id().event();

    //presumably this whole part can be removed!

    neutrinosLepB.clear();
    neutrinosLepB_C.clear();
    gToBB.clear();
    gToCC.clear();
    alltaus_.clear();
    Bhadron_.clear();
    Bhadron_daughter_.clear();

    gen_particle_pt.clear();
    gen_particle_eta.clear();
    gen_particle_phi.clear();
    gen_particle_mass.clear();
    gen_particle_mother_id.clear();
    gen_particle_id.clear();
    gen_particle_status.clear();
    gen_particle_daughters_id.clear();
    gen_particle_daughters_igen.clear();
    gen_particle_daughters_status.clear();
    gen_particle_daughters_pt.clear();
    gen_particle_daughters_eta.clear();
    gen_particle_daughters_phi.clear();
    gen_particle_daughters_mass.clear();
    gen_particle_daughters_charge.clear();
    genLepFromResonance4V_.clear();
    genMuonsFromResonance4V_.clear();
    genElectronsFromResonance4V_.clear();
    tau_gen_visible_.clear();
    tau_gen_.clear();
    tau_gen_type_.clear();
    
    genBmeson_daughter_pt_.clear();
    genBmeson_daughter_eta_.clear();
    genBmeson_daughter_phi_.clear();
    genBmeson_daughter_pdgId_.clear();
    genBmeson_daughter_mesonIdx_.clear();
    genBmeson_pt_.clear();
    genBmeson_eta_.clear();
    genBmeson_phi_.clear();
    genBmeson_pdgId_.clear();
    genCmeson_daughter_pt_.clear();
    genCmeson_daughter_eta_.clear();
    genCmeson_daughter_phi_.clear();
    genCmeson_daughter_pdgId_.clear();
    genCmeson_daughter_mesonIdx_.clear();
    genCmeson_pt_.clear();
    genCmeson_eta_.clear();
    genCmeson_phi_.clear();
    genCmeson_pdgId_.clear();

    constituent_pt_.clear();
    constituent_eta_.clear();
    constituent_phi_.clear();
    constituent_pdgId_.clear();
    constituent_BdaughterDr_.clear();
    constituent_BdaughterIdx_.clear();
    constituent_CdaughterDr_.clear();
    constituent_CdaughterIdx_.clear();

    nMatchedDaughtersB_=0;
    nMatchedDaughtersC_=0;
    isMatchedB_=0;
    isMatchedBB_=0;
    isNotMatched_=0;
    isMatchedC_=0;
    isMatchedBAndC_=0;

    isMatchedB1d_=0;
    isMatchedB2d_=0;
    isMatchedB3d_=0;
    isMatchedBMore3d_=0;
    isMatchedBB2d_=0;
    isMatchedBB3d_=0;
    isMatchedBB4d_=0;
    isMatchedBBMore4d_=0;
    nConstituent_=0;
    nGenBmeson_=0;
    nGenCmeson_=0;
    isMatchedC1d_=0;
    isMatchedC2d_=0;
    isMatchedC3d_=0;
    isMatchedCMore3d_=0;
    isMatchedB1dC1d_=0;
    isMatchedB2dC1d_=0;
    isMatchedBMore2dC1d_=0;
    isMatchedB1dC2d_=0;
    isMatchedB2dC2d_=0;
    isMatchedBMore2dC2d_=0;
    isMatchedB1dCMore2d_=0;
    isMatchedB2dCMore2d_=0;
    isMatchedBMore2dCMore2d_=0;



    jetv_gen_wnu.clear();
    jetv_gen.clear();

    PatRefPtSorter<reco::GenJetRef> genJetRefSorter;

    // Standard gen-jets excluding the neutrinos
    if(genJetsH.isValid()){
      for (auto jets_iter = genJetsH->begin(); jets_iter != genJetsH->end(); ++jets_iter) {                                                                                                   
	reco::GenJetRef jref (genJetsH,jets_iter-genJetsH->begin());                                                                                                                      
	jetv_gen.push_back(jref);                                                                                                                                                              
      }
      sort(jetv_gen.begin(), jetv_gen.end(), genJetRefSorter);
    }
    
    // GEN jets with neutrinos
    if(genJetsWnuH.isValid()){
      for (auto jets_iter = genJetsWnuH->begin(); jets_iter != genJetsWnuH->end(); ++jets_iter) {                                                                                           
	reco::GenJetRef jref  (genJetsWnuH, jets_iter-genJetsWnuH->begin());                                                                                                                 
	jetv_gen_wnu.push_back(jref);                                                                                                                                                              
      }
      sort(jetv_gen_wnu.begin(), jetv_gen_wnu.end(), genJetRefSorter);
      }

 for (const reco::Candidate &genC : *genParticlesHandle)
   {
     const reco::GenParticle &gen = static_cast< const reco::GenParticle &>(genC);
     
     if((abs(gen.pdgId())>500&&abs(gen.pdgId())<600)||(abs(gen.pdgId())>5000&&abs(gen.pdgId())<6000)) {

       Bhadron_.push_back(gen);
       if(gen.numberOfDaughters()>0){
     
	 if( (abs(gen.daughter(0)->pdgId())>500&&abs(gen.daughter(0)->pdgId())<600)||(abs(gen.daughter(0)->pdgId())>5000&&abs(gen.daughter(0)->pdgId())<6000))
	   {
	     if(gen.daughter(0)->numberOfDaughters()>0)
	       {
		
		 const reco::GenParticle &daughter_ = static_cast< const reco::GenParticle &>(*(gen.daughter(0)->daughter(0)));
		 
		 if(daughter_.vx()!=gen.vx())
		   { 
		     Bhadron_daughter_.push_back(daughter_);
		   }
                 else Bhadron_daughter_.push_back(gen);
	       }
	     else  Bhadron_daughter_.push_back(gen);
	     
	   }
	 else{
	   const reco::GenParticle &daughter_ = static_cast< const reco::GenParticle &>(*gen.daughter(0));
	   Bhadron_daughter_.push_back(daughter_);
	 }

       }// if daughter is there
       else {
	 Bhadron_daughter_.push_back(gen);
       }
     }
   }

   for (auto gens_iter = genParticlesHandle->begin(); gens_iter != genParticlesHandle->end(); ++gens_iter) {
      if ( abs(gens_iter->pdgId())!=11 and abs(gens_iter->pdgId())!=13 
           and abs(gens_iter->pdgId())!=211 and abs(gens_iter->pdgId())!=321
         )
           continue;
      if ( !gens_iter->isLastCopy() )
           continue;
      const reco::GenParticle* mother =  static_cast<const reco::GenParticle*> (gens_iter->mother());
      if (mother==NULL)
          continue;
      bool FromB=false;
      if ( (abs(mother->pdgId())>500 and abs(mother->pdgId())<600) or (abs(mother->pdgId())>5000 and abs(mother->pdgId())<6000) )
         FromB=true;

      while (!FromB) {
         mother =  static_cast< const reco::GenParticle*> (mother->mother());
         if ( mother==NULL)   break;
         if ( (abs(mother->pdgId())>500 and abs(mother->pdgId())<600) or (abs(mother->pdgId())>5000 and abs(mother->pdgId())<6000) ){
            FromB=true;
            break;
         }
       }

      if (FromB) {        
         genBmeson_daughter_pt_.emplace_back(gens_iter->pt());
         genBmeson_daughter_eta_.emplace_back(gens_iter->eta());
         genBmeson_daughter_phi_.emplace_back(gens_iter->phi());
         genBmeson_daughter_pdgId_.emplace_back(gens_iter->pdgId());
         genBmeson_daughter_mesonIdx_.emplace_back( mother - genParticlesHandle->data());
         genBmeson_pt_.emplace_back( mother->pt());
         genBmeson_eta_.emplace_back( mother->eta());
         genBmeson_phi_.emplace_back( mother->phi());
         genBmeson_pdgId_.emplace_back( mother->pdgId());
      } else {
         mother =  static_cast<const reco::GenParticle*> (gens_iter->mother());
         bool FromC=false;      
         if ( (abs(mother->pdgId())>400 and abs(mother->pdgId())<500) or (abs(mother->pdgId())>4000 and abs(mother->pdgId())<5000) )
            FromC=true;
         while (!FromC) {
            mother =  static_cast< const reco::GenParticle*> (mother->mother());
            if ( mother==NULL)   break;
            if ( (abs(mother->pdgId())>400 and abs(mother->pdgId())<500) or (abs(mother->pdgId())>4000 and abs(mother->pdgId())<5000) ){
               FromC=true;
               break;
            }
         }
         if (FromC) {
            genCmeson_daughter_pt_.emplace_back(gens_iter->pt());
            genCmeson_daughter_eta_.emplace_back(gens_iter->eta());
            genCmeson_daughter_phi_.emplace_back(gens_iter->phi());
            genCmeson_daughter_pdgId_.emplace_back(gens_iter->pdgId());
            genCmeson_daughter_mesonIdx_.emplace_back( mother - genParticlesHandle->data());
            genCmeson_pt_.emplace_back( mother->pt());
            genCmeson_eta_.emplace_back( mother->eta());
            genCmeson_phi_.emplace_back( mother->phi());
            genCmeson_pdgId_.emplace_back( mother->pdgId());
         }  
      } // else fromB
  } //loop on gen parts 
  nGenBmeson_=genBmeson_daughter_pt_.size();
  nGenCmeson_=genCmeson_daughter_pt_.size();
  for (const reco::Candidate &genC : *genParticlesHandle) {
        const reco::GenParticle &gen = static_cast< const reco::GenParticle &>(genC);
        if(abs(gen.pdgId())==12||abs(gen.pdgId())==14||abs(gen.pdgId())==16) {
            const reco::GenParticle* mother =  static_cast< const reco::GenParticle*> (gen.mother());
            if(mother!=NULL) {
                if((abs(mother->pdgId())>500&&abs(mother->pdgId())<600)||(abs(mother->pdgId())>5000&&abs(mother->pdgId())<6000)) {
                    neutrinosLepB.emplace_back(gen);
                }
                if((abs(mother->pdgId())>400&&abs(mother->pdgId())<500)||(abs(mother->pdgId())>4000&&abs(mother->pdgId())<5000)) {
                    neutrinosLepB_C.emplace_back(gen);
                }
            }
            else {
                std::cout << "No mother" << std::endl;
            }
        }

        int id(std::abs(gen.pdgId())); 
        int status(gen.status());

        if (id == 21 && status >= 21 && status <= 59) { //// Pythia8 hard scatter, ISR, or FSR
            if ( gen.numberOfDaughters() == 2 ) {
                const reco::Candidate* d0 = gen.daughter(0);
                const reco::Candidate* d1 = gen.daughter(1);
                if ( std::abs(d0->pdgId()) == 5 && std::abs(d1->pdgId()) == 5
                        && d0->pdgId()*d1->pdgId() < 0 && reco::deltaR(*d0, *d1) < 0.4) gToBB.push_back(gen) ;
                if ( std::abs(d0->pdgId()) == 4 && std::abs(d1->pdgId()) == 4
                        && d0->pdgId()*d1->pdgId() < 0 && reco::deltaR(*d0, *d1) < 0.4) gToCC.push_back(gen) ;
            }
        }

        if(id == 15 && false){
            alltaus_.push_back(gen);
        }

 }

  // GEN particle information
 if(genParticlesHandle.isValid()){
   unsigned int igen = 0;
   for (auto gens_iter = genParticlesHandle->begin(); gens_iter != genParticlesHandle->end(); ++gens_iter) {      
     if((abs(gens_iter->pdgId()) == 25 or abs(gens_iter->pdgId()) == 24 or abs(gens_iter->pdgId()) == 23) and
	gens_iter->isLastCopy() and gens_iter->statusFlags().fromHardProcess()){ 

       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());
       gen_particle_mother_id.push_back(-999);  

       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }

     // Bs mesons
     if(abs(gens_iter->pdgId()) == 531 and
	gens_iter->isLastCopy()){
       
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());
       
       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }
     
     // Final states Leptons (e,mu) and Neutrinos --> exclude taus. They need to be prompt or from Tau decay      
     if (abs(gens_iter->pdgId()) > 10 and abs(gens_iter->pdgId()) < 17 and abs(gens_iter->pdgId()) != 15  and 
	 (gens_iter->isPromptFinalState() or gens_iter->isDirectPromptTauDecayProductFinalState())) { 

       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());
       gen_particle_mother_id.push_back(-999);
       // No need to save daughters here
       igen++;
     }

     // Final state quarks or gluons from the hard process before the shower --> partons in which H/Z/W/top decay into
     if (((abs(gens_iter->pdgId()) >= 1 and abs(gens_iter->pdgId()) <= 5) or abs(gens_iter->pdgId()) == 21) and 
	 gens_iter->statusFlags().fromHardProcess() and gens_iter->statusFlags().isFirstCopy()){
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());
       gen_particle_mother_id.push_back(-999);  
       igen++;
       // no need to save daughters
     }

     // Special case of taus: last-copy, from hard process and, prompt and decayed
     if(abs(gens_iter->pdgId()) == 15 and gens_iter->isLastCopy()){

       // remove the hardprocess requirement, since we want taus from Bs and not
       //gens_iter->statusFlags().fromHardProcess() and gens_iter->isPromptDecayed()){ 

       // hadronic taus
       gen_particle_pt.push_back(gens_iter->pt());
       gen_particle_eta.push_back(gens_iter->eta());
       gen_particle_phi.push_back(gens_iter->phi());
       gen_particle_mass.push_back(gens_iter->mass());
       gen_particle_id.push_back(gens_iter->pdgId());
       gen_particle_status.push_back(gens_iter->status());

       // save mother
       const reco::Candidate* mother = gens_iter->mother();
       if (mother) {
	 gen_particle_mother_id.push_back(abs(mother->pdgId()));
       } else {
	 gen_particle_mother_id.push_back(-999);  
       }

       if(debug){std::cout<<"Saved gen tau "<<gens_iter->pt()<<" "<<gens_iter->eta()<<" "<<gens_iter->mass()<<" mother "<<abs(mother->pdgId())<<std::endl;}
       
       // only store the final decay particles
       for(size_t idau = 0; idau < gens_iter->numberOfDaughters(); idau++){
	 if(not dynamic_cast<const reco::GenParticle*>(gens_iter->daughter(idau))->statusFlags().isTauDecayProduct()) continue;
	 if(debug){std::cout<<"I am daughter of tau "<<gens_iter->daughter(idau)->pdgId()<<std::endl;}
	 gen_particle_daughters_id.push_back(gens_iter->daughter(idau)->pdgId());
	 gen_particle_daughters_igen.push_back(igen);
	 gen_particle_daughters_pt.push_back(gens_iter->daughter(idau)->pt());
	 gen_particle_daughters_eta.push_back(gens_iter->daughter(idau)->eta());
	 gen_particle_daughters_phi.push_back(gens_iter->daughter(idau)->phi());
	 gen_particle_daughters_mass.push_back(gens_iter->daughter(idau)->mass());    
	 gen_particle_daughters_status.push_back(gens_iter->daughter(idau)->status());
	 gen_particle_daughters_charge.push_back(gens_iter->daughter(idau)->charge());
       }
       igen++;
     }  
   }
 }

}

//use either of these functions

bool ntuple_JetInfo::fillBranches(const pat::Jet & jet, const size_t& jetidx, const edm::View<pat::Jet> * coll ){
    if(!coll)
        throw std::runtime_error("ntuple_JetInfo::fillBranches: no jet collection");

    isMC_    = -1;
    isemu_   =  0;
    isdimu_  =  0;
    ismutau_ =  0;
    /// thresholds for matching
    static float dRCone        = 0.4;
    static float dRMatchingPF  = 0.1;
    static float ptGenLeptonMin = 8;
    static float ptGenTauVisibleMin = 5;

    // Gen leptons from resonance decay 
    std::vector<TLorentzVector> genLepFromResonance4V;
    std::vector<TLorentzVector> genMuonsFromResonance4V;
    std::vector<TLorentzVector> genElectronsFromResonance4V;
    std::vector<int> genMuonsFromResonanceCharge;
    std::vector<int> genElectronsFromResonanceCharge;
    std::vector<TLorentzVector> tau_gen_visible;
    std::vector<TLorentzVector> tau_gen;
    std::vector<int> tau_gen_type; // 0: hadronic, 1: muon, 2: electron
    std::vector<int> tau_gen_charge;
    std::vector<unsigned int> tau_gen_nch;
    std::vector<unsigned int> tau_gen_np0;
    std::vector<unsigned int> tau_gen_nnh;
    std::vector<Resonance> resonances;

    

    if(debug){std::cout<<"##### Jet idx "<<jetidx<<" jet pt  "<<jet.pt()<<" jet eta "<<jet.eta()<<std::endl;}

    for(size_t igen = 0; igen < gen_particle_pt.size(); igen++){
      // select resonances like Higgs, W, Z, taus
      if(abs(gen_particle_id.at(igen)) == 25 or
	 abs(gen_particle_id.at(igen)) == 23 or
	 abs(gen_particle_id.at(igen)) == 24 or
	 abs(gen_particle_id.at(igen)) == 15){
	for(size_t idau = 0; idau < gen_particle_daughters_id.size(); idau++){
	  // select electrons or muons from the resonance / tau decay
	  if(gen_particle_daughters_igen.at(idau) == igen and
	     (abs(gen_particle_daughters_id.at(idau)) == 11 or
	      abs(gen_particle_daughters_id.at(idau)) == 13)){
	    TLorentzVector gen4V;
	    gen4V.SetPtEtaPhiM(gen_particle_daughters_pt.at(idau),gen_particle_daughters_eta.at(idau),gen_particle_daughters_phi.at(idau),gen_particle_daughters_mass.at(idau));
	    if(std::find(genLepFromResonance4V.begin(),genLepFromResonance4V.end(),gen4V) == genLepFromResonance4V.end())
	      genLepFromResonance4V.push_back(gen4V);
	    if(abs(gen_particle_daughters_id.at(idau)) == 13 and 
	       std::find(genMuonsFromResonance4V.begin(),genMuonsFromResonance4V.end(),gen4V) == genMuonsFromResonance4V.end()){
	      genMuonsFromResonance4V.push_back(gen4V);
	      genMuonsFromResonanceCharge.push_back(gen_particle_daughters_charge.at(idau));
	    }
	    if(abs(gen_particle_daughters_id.at(idau)) == 11 and 
	       std::find(genElectronsFromResonance4V.begin(),genElectronsFromResonance4V.end(),gen4V) == genElectronsFromResonance4V.end()){
	      genElectronsFromResonance4V.push_back(gen4V);
	      genElectronsFromResonanceCharge.push_back(gen_particle_daughters_charge.at(idau));
	    }
	  }
	}
      }
    }


    // Gen hadronic taus
    for(size_t igen = 0; igen < gen_particle_pt.size(); igen++){
      if(abs(gen_particle_id.at(igen)) == 15){ // hadronic or leptonic tau

	if(debug){std::cout<<" Now create gen visible tau "<<gen_particle_pt.at(igen)<<std::endl;}
	TLorentzVector tau_gen_tmp;
	unsigned int tau_gen_nch_tmp = 0;
	unsigned int tau_gen_np0_tmp = 0;
	unsigned int tau_gen_nnh_tmp = 0;
	for(size_t idau = 0; idau < gen_particle_daughters_pt.size(); idau++){
	  if(gen_particle_daughters_igen.at(idau) == igen and
	     abs(gen_particle_daughters_id.at(idau)) != 11 and // no mu
	     abs(gen_particle_daughters_id.at(idau)) != 13 and // no el
	     abs(gen_particle_daughters_id.at(idau)) != 12 and // no neutrinos
	     abs(gen_particle_daughters_id.at(idau)) != 14 and
	     abs(gen_particle_daughters_id.at(idau)) != 16){
	    TLorentzVector tmp4V; 
	    tmp4V.SetPtEtaPhiM(gen_particle_daughters_pt.at(idau),gen_particle_daughters_eta.at(idau),gen_particle_daughters_phi.at(idau),gen_particle_daughters_mass.at(idau));
	    tau_gen_tmp += tmp4V;
	    if (gen_particle_daughters_charge.at(idau) != 0 and gen_particle_daughters_status.at(idau) == 1) tau_gen_nch_tmp ++; // charged particles
	    else if(gen_particle_daughters_charge.at(idau) == 0 and gen_particle_daughters_id.at(idau) == 111) tau_gen_np0_tmp++;
	    else if(gen_particle_daughters_charge.at(idau) == 0 and gen_particle_daughters_id.at(idau) != 111) tau_gen_nnh_tmp++;
	  }
	}
	if(tau_gen_tmp.Pt() > 5){ // good hadronic tau
	  tau_gen_visible.push_back(tau_gen_tmp);
	  tau_gen_tmp.SetPtEtaPhiM(gen_particle_pt.at(igen),gen_particle_eta.at(igen),gen_particle_phi.at(igen),gen_particle_mass.at(igen));
	  tau_gen_charge.push_back((gen_particle_id.at(igen) > 0) ? -1 : 1);
	  tau_gen.push_back(tau_gen_tmp);
	  tau_gen_nch.push_back(tau_gen_nch_tmp);
	  tau_gen_np0.push_back(tau_gen_np0_tmp);
	  tau_gen_nnh.push_back(tau_gen_nnh_tmp);
	}
      }
    }




    // Build Bs-> di-tau resonance
    std::vector<unsigned int> skipTaus;
    for(size_t igen = 0; igen < (gen_particle_pt).size(); igen++){
      if(abs((gen_particle_id).at(igen)) == 531){ // gen particle is a bs
	TLorentzVector resonance4V; // save resonance 4V
	resonance4V.SetPtEtaPhiM((gen_particle_pt).at(igen),(gen_particle_eta).at(igen) \
,(gen_particle_phi).at(igen),(gen_particle_mass).at(igen));
	// find the daughters of the Bs and check they are taus
	if (debug){std::cout<< "--->Bs found, looking for taus"<<std::endl;}
	std::vector<unsigned int> daughters;
	bool is_tau_mu = false;
	bool is_tau_e = false;
	for(size_t idau = 0; idau < (gen_particle_daughters_id).size(); idau++){
	  if((gen_particle_daughters_igen).at(idau) == igen){ //check bs daughters
	    if(abs((gen_particle_daughters_id).at(idau)) == 15){ // it's a tau
	      TLorentzVector daughter4V; // define the 4v vector to find the right tau in the already saved collection
	      daughter4V.SetPtEtaPhiM((gen_particle_daughters_pt).at(idau),(gen_particle_daughters_eta).at(idau),(gen_particle_daughters_phi).at(idau),(gen_particle_daughters_mass).at(idau));
	      if(debug){std::cout<<" bs daughter pT "<<gen_particle_daughters_pt.at(idau)<<std::endl;}
	      int tauIndex = -1;
	      daughters.push_back(idau);
	      if(debug){std::cout<<"Size of hadronic taus collection "<<tau_gen_visible.size()<<std::endl;}
	      for(size_t itau = 0; itau < tau_gen_visible.size(); itau++){ // find which hadronic tau
		if(debug){std::cout<<"hadronic tau n "<<itau<<" is it bs daughter? "<<tau_gen.at(itau).DeltaR(daughter4V)<<" pt "<<tau_gen_visible.at(itau).Pt()<<" full pT "<<tau_gen.at(itau).Pt()<<std::endl;}
		if(tau_gen.at(itau).DeltaR(daughter4V) == 0 and std::find(skipTaus.begin(), skipTaus.end(),itau) == skipTaus.end()){
		  tauIndex = itau;
		  // add here something to save! I dont know....
		}
	      }
	      if (tauIndex != -1){ // there was a match with an hadronic tau
		skipTaus.push_back(tauIndex);
		if (debug){std::cout<< "tau decaying hadronically found"<<std::endl;}
	      }
	      // if not hadronic tau, then find out how the tau decays: mu or e
	      else{
		for(size_t igen2 = 0; igen2 < (gen_particle_pt).size(); igen2++){
		  if(abs((gen_particle_id).at(igen2)) == 15){ //looking for taus
		    TLorentzVector gentau4V;
		    gentau4V.SetPtEtaPhiM((gen_particle_pt).at(igen2),(gen_particle_eta).at(igen2),(gen_particle_phi).at(igen2),(gen_particle_mass).at(igen2));
                      if(gentau4V.DeltaR(daughter4V) == 0){     // it is exactly the tau I am looking for
			if (debug){std::cout<< "tau decaying leptonically found"<<std::endl;}

                        if (debug){std::cout<< "found the corresponding gen tau "<<igen2<<std::endl;} //
                        for(size_t idau2 = 0; idau2 < (gen_particle_daughters_id).size(); idau2++){
                          if((gen_particle_daughters_igen).at(idau2) == igen2){ // look for  tau daughters
                            if (debug){std::cout<< "gen tau daughters"<<(gen_particle_daughters_id).at(idau2)<<std::endl;}
                            if(abs((gen_particle_daughters_id).at(idau2)) == 11){ // it's an electron
                              if (debug){std::cout<< "tau decays in electron"<<std::endl;}
                              is_tau_e = true;
			    }
                            else if(abs((gen_particle_daughters_id).at(idau2)) == 13){ // it's an electron
                              is_tau_mu = true;
                              if (debug){std::cout<< "tau decays in muon"<<std::endl;}
                            }
                          }
                        }
                      }
                    }
                  }
                }// else
              } // if it's a tau
            }
          }// loop on bs daughters
          // tau_h tau_X, therefore I require at least 1 tau_h, at max 2 taus
          //if(skipTaus.size()<1 or daughters.size()!=2){
          if(daughters.size()!=2){
            continue;
          }

	  // if bs-> tau tau then I can save it (maybe it's useful to also save the taus?)
	  if(skipTaus.size()==0){ // bs-> tau_X tau_X OR the hadronic tau too slow
	    continue; // we don't save the resonance if not tau_h tau_X 
	  }
          resonances.push_back(Resonance()); // Class
          resonances.back().p4 = resonance4V;
          if(skipTaus.size()==2){ // bs-> tau_h tau_h
            resonances.back().daugdecay = 1;
          }
          else if(skipTaus.size()==1 and is_tau_mu){ // bs-> tau_h tau_mu
            resonances.back().daugdecay = 2;
          }
          else if(skipTaus.size()==1 and is_tau_e){ // bs-> tau_h tau_e
            resonances.back().daugdecay = 3;
          }
          if (debug){std::cout<< "resonance decay "<<resonances.back().daugdecay<<std::endl;}
      }
    }



    
    // matching with gen-leptons (muons/electrons/hadronic taus)
    float minDR = 1000;
    int nlep_in_cone  = 0;
    int pos_matched_genmu = -1;
    int pos_matched_genele = -1;
    int pos_matched_tauh = -1;
    int pos_matched_tauhtauh = -1; // for ditau label
    int pos_matched_tauhtaumu = -1; // for ditau label
    int pos_matched_tauhtaue = -1; // for ditau label
    int gentau_decaymode = -1;   
    TLorentzVector genLepton4V;
    TLorentzVector genLeptonVis4V;

    TLorentzVector jet4V;
    jet4V.SetPtEtaPhiM(jet.pt(),jet.eta(),jet.phi(),jet.mass());


    // matching jet- ditau
    
    float mindR_jetres = 1000;
    for(size_t ipair = 0; ipair < resonances.size(); ipair++){ // find the bs that better maches with the reco ak4jet
      float dRjres = jet4V.DeltaR(resonances.at(ipair).p4);
      if(dRjres < 0.4 and dRjres < mindR_jetres){
	//match_index_dR_resonance_jet = ipair;
	if(debug){std::cout<<"??? it is matching!! dR "<<dRjres<<std::endl;}
	if( resonances.at(ipair).daugdecay==1){
	  pos_matched_tauhtauh = 1;
	}
	else if ( resonances.at(ipair).daugdecay==2){
	  pos_matched_tauhtaumu = 1;
	}
	else if ( resonances.at(ipair).daugdecay==3){
	  pos_matched_tauhtaue = 1;
	}
	if(debug){std::cout<<"pos_matched_tauhtauh "<<pos_matched_tauhtauh<<" pos_matched_tauhtaumu "<<pos_matched_tauhtaumu<<" pos_matched_tauhtaue "<<pos_matched_tauhtaue<<std::endl;}
      }
    }

    // single tau only for background samples (it is easier so I am sure there are no bs-> tautau)
    if(resonances.size()<1 and pos_matched_tauhtauh == -1 and pos_matched_tauhtaumu == -1 and  pos_matched_tauhtaue == -1){

      // Matching jet-single tau
      if(debug){std::cout<<" Not di-tau and not Bs  "<<minDR<<" "<<tau_gen_visible.size()<<std::endl;}
      for(size_t itau = 0; itau < tau_gen_visible.size(); itau++){
	float dR = tau_gen_visible.at(itau).DeltaR(jet4V);
	if(debug){std::cout<< "Tau?"<<dR<<" "<<minDR<<std::endl;}
	if(dR < 0.4) nlep_in_cone++;
	if(dR < 0.4 and dR < minDR){
	  pos_matched_tauh = itau;
	  minDR = dR;
	  if(debug){std::cout<< "This is matching!! "<<dR<<" "<<pos_matched_tauh<<std::endl;}
	  genLeptonVis4V = tau_gen_visible.at(itau);
	  gentau_decaymode = 5*(tau_gen_nch.at(itau)-1)+tau_gen_np0.at(itau);
	}
      }
    }

    /*
    skip_jet_ = 0;
    

    for(size_t igen = 0; igen < genMuonsFromResonance4V.size(); igen++){
      float dR = jet4V.DeltaR(genMuonsFromResonance4V.at(igen));      
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and genMuonsFromResonance4V.at(igen).Pt() >= ptGenLeptonMin){
	pos_matched_genmu = igen;
	minDR = dR;
	genLepton4V = genMuonsFromResonance4V.at(igen);
	genLeptonVis4V = genMuonsFromResonance4V.at(igen);
      }
    }
    
    for(size_t igen = 0; igen < genElectronsFromResonance4V.size(); igen++){
      float dR = jet4V.DeltaR(genElectronsFromResonance4V.at(igen));      
      if(dR < dRCone) nlep_in_cone++;
      if(dR < dRCone and dR < minDR and genElectronsFromResonance4V.at(igen).Pt() >= ptGenLeptonMin){
	pos_matched_genmu  = -1;
	pos_matched_genele = igen;
	minDR = dR;
	genLepton4V = genElectronsFromResonance4V.at(igen);
	genLeptonVis4V = genElectronsFromResonance4V.at(igen);
      }
    }

    // copy of the real used_gentaus
    std::set<int> tmp_used_gentaus_ = *used_gentaus_;
    for(size_t itau = 0; itau < tau_gen_visible.size(); itau++){      

      if(tau_gen_type.at(itau) != 0) continue; // skip if not hadronic tau!
      
      if(tmp_used_gentaus_.find(itau) != tmp_used_gentaus_.end()) {
	//std::cout<<"skip tau "<<itau<<std::endl;
	continue; // già usato, skip
      }
      
      float dR = tau_gen_visible.at(itau).DeltaR(jet4V); 
      //std::cout<<"gen visible tau  "<<itau<<" pt "<<tau_gen.at(itau).Pt()<<" visible pt "<<tau_gen_visible.at(itau).Pt()<<" dr with jet "<<dR<<" min dR "<<minDR<<std::endl;
      //std::cout<<"infos jet and tau for dR computation "<<jet.eta()<<" "<<jet.phi()<<" "<<tau_gen_visible.at(itau).Eta()<<" "<<tau_gen_visible.at(itau).Phi()<<std::endl;
      if(dR < 0.4) nlep_in_cone++;
      if(dR < 0.4 and dR < minDR and pos_matched_tauh==-1 and tau_gen_visible.at(itau).Pt() >= ptGenTauVisibleMin){
	pos_matched_genmu  = -1;
	pos_matched_genele = -1;
	pos_matched_tauh = itau;
	minDR = dR;
	gentau_decaymode = 5*(tau_gen_nch.at(itau)-1)+tau_gen_np0.at(itau);
	genLepton4V = tau_gen.at(itau);
	genLeptonVis4V = tau_gen_visible.at(itau);
	tmp_used_gentaus_.insert(itau);  // mark this tau as already matched
	//std::cout<<" first tau "<<itau<<" "<<dR<<std::endl;
      }
    }
    // Print all elements
    //std::cout<<"used gen taus "<<std::endl;
    // di-tau matching

    for(size_t itau = 0; itau < tau_gen_visible.size(); itau++){

      //std::cout<<"other taus beofre filter  "<<itau<<" pt "<<tau_gen.at(itau).Pt()<<std::endl;
      
      if(tmp_used_gentaus_.find(itau) != tmp_used_gentaus_.end()) continue; // già usato, skip

      float dR = tau_gen_visible.at(itau).DeltaR(jet4V);
      //std::cout<<"other taus  "<<itau<<" pt "<<tau_gen.at(itau).Pt()<<" visible pt "<<tau_gen_visible.at(itau).Pt()<<" dr with jet "<<dR<<" min dR "<<minDR<<" is hadronic? "<<tau_gen_type.at(itau)<<std::endl;
      //std::cout<<"infos jet and tau for dR computation "<<jet.eta()<<" "<<jet.phi()<<" "<<tau_gen_visible.at(itau).Eta()<<" "<<tau_gen_visible.at(itau).Phi()<<std::endl;
      if(dR < 0.4) nlep_in_cone++;
      if(pos_matched_tauh != -1 and pos_matched_ditauh == -1 and dR!=minDR  and dR < 0.4){ // this is the second tau to be matched with the jet
	//std::cout<<" second tau "<<itau<<" "<<dR<<std::endl;

	pos_matched_ditauh = itau; // the other tau is already saved
	tmp_used_gentaus_.insert(itau);  // mark this tau as matched
	genditau_decaymode = tau_gen_type.at(itau);

      }
      else if (pos_matched_ditauh != -1 and dR!=minDR and dR < 0.4){
	//std::cout<<" Too many taus, so jet skipped "<<std::endl;
	skip_jet_ = 1;
	tmp_used_gentaus_.insert(itau);  // mark this tau as matched
      }
    }
    */
    jet_genmatch_lep_vis_pt_ = genLeptonVis4V.Pt();

    PatPtSorter<pat::Tau>      tauSorter;  
    pat::TauCollection tausColl = *tausH;
    
    // Sorting taus based on pT
    sort(tausColl.begin(),tausColl.end(),tauSorter);
    std::vector<math::XYZTLorentzVector> tau_pfcandidates;  // in order to match PF and HPS candidates

    for (size_t i = 0; i < tausColl.size(); i++) {
      for(unsigned ipart = 0; ipart < tausColl[i].signalCands().size(); ipart++){
	const pat::PackedCandidate* pfcand = dynamic_cast<const pat::PackedCandidate*> (tausColl[i].signalCands()[ipart].get());      
	tau_pfcandidates.push_back(pfcand->p4());
      }
    }
    
    TLorentzVector tau4V_fromPF;
    std::vector<TLorentzVector> muon4V_fromPF;
    std::vector<TLorentzVector> electron4V_fromPF;

    for (unsigned int i = 0; i <  jet.numberOfDaughters(); i++){
      const pat::PackedCandidate* PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
      if(!PackedCandidate_) continue;
      if(PackedCandidate_->pt() < min_candidate_pt_) continue; 
      
      if (abs(PackedCandidate_->pdgId()) == 11){
	TLorentzVector tmp4V;
	tmp4V.SetPtEtaPhiM(PackedCandidate_->pt(),PackedCandidate_->eta(),PackedCandidate_->phi(),PackedCandidate_->mass());
	electron4V_fromPF.push_back(tmp4V);
      }
      if (abs(PackedCandidate_->pdgId()) == 13){
	TLorentzVector tmp4V;
	tmp4V.SetPtEtaPhiM(PackedCandidate_->pt(),PackedCandidate_->eta(),PackedCandidate_->phi(),PackedCandidate_->mass());
	muon4V_fromPF.push_back(tmp4V);
      }
      if(std::find(tau_pfcandidates.begin(),tau_pfcandidates.end(),PackedCandidate_->p4()) != tau_pfcandidates.end()){
	TLorentzVector tmp4V;
	tmp4V.SetPtEtaPhiM(PackedCandidate_->pt(),PackedCandidate_->eta(),PackedCandidate_->phi(),PackedCandidate_->mass());
	tau4V_fromPF += tmp4V;
      }
    }
    
    // Matching with muon by considering only the highest pt one
    std::sort(muon4V_fromPF.begin(),muon4V_fromPF.end(),
	      [&](const TLorentzVector & a, const TLorentzVector & b){
		return a.Pt() > b.Pt();
	      });
    
    minDR = 1000;
    int pos_matched_reco_mu = -1;
    if(not muon4V_fromPF.empty()){
      for(size_t imu = 0; imu < muon4V_fromPF.size(); imu++){
	TLorentzVector tmp4V;
	tmp4V = muon4V_fromPF.at(imu);
	float dR = tmp4V.DeltaR(jet4V);
	float dRm = muon4V_fromPF.front().DeltaR(tmp4V);
	if(dR < dRCone and dRm < dRMatchingPF and dRm < minDR){
	  pos_matched_reco_mu = imu;
	  minDR = dRm;	      
	}
      }
    }
    if(pos_matched_reco_mu >= 0){
      jet_mumatch_pt_ = muon4V_fromPF.at(pos_matched_reco_mu).Pt();
    }
    else if(not muon4V_fromPF.empty()){
      jet_mumatch_pt_ = muon4V_fromPF.front().Pt();
    }
    else{
      jet_mumatch_pt_ = -1;
    }

    std::sort(electron4V_fromPF.begin(),electron4V_fromPF.end(),
	      [&](const TLorentzVector & a, const TLorentzVector & b){
		return a.Pt() > b.Pt();
	      });
    
    minDR = 1000;
    int pos_matched_reco_ele = -1;
    if(not electron4V_fromPF.empty()){
      for(size_t imu = 0; imu < electron4V_fromPF.size(); imu++){
	TLorentzVector tmp4V;
	tmp4V = electron4V_fromPF.at(imu);
	float dR = tmp4V.DeltaR(jet4V);
	float dRm = electron4V_fromPF.front().DeltaR(tmp4V);
	if(dR < dRCone and dRm < dRMatchingPF and dRm < minDR){
	  pos_matched_reco_ele = imu;
	  minDR = dRm;	      
	}
      }
    }
    if(pos_matched_reco_ele >= 0){
      jet_elematch_pt_ = electron4V_fromPF.at(pos_matched_reco_ele).Pt();
    }
    else if(not electron4V_fromPF.empty()){
      jet_elematch_pt_ = electron4V_fromPF.front().Pt();
    }
    else{
      jet_elematch_pt_ = -1;
    }

    // Matching with reco tau
    minDR = 1000;
    int pos_matched_reco_tauh = -1;
    for (size_t itau = 0; itau < tausColl.size(); itau++) {
      TLorentzVector tmp4V;
      tmp4V.SetPtEtaPhiM(tausColl[itau].pt(),tausColl[itau].eta(),tausColl[itau].phi(),tausColl[itau].mass());
      float dR = tmp4V.DeltaR(jet4V);
      float dRt = tmp4V.DeltaR(tau4V_fromPF);	      
      if (dR < dRCone and dRt < dRMatchingPF and dRt < minDR){ // small numerical roundoff due to float to double approximation
	pos_matched_reco_tauh = itau;
	minDR = dR;
      }
    }
    if(pos_matched_reco_tauh >= 0){
      jet_taumatch_pt_ = tausColl[pos_matched_reco_tauh].pt();
    }
    else{
      jet_taumatch_pt_ = -1;
    }

    /// cuts ///
    bool returnval=true;
    
    // some cuts to contrin training region
    if ( jet.pt() < jetPtMin_ ||  jet.pt() > jetPtMax_ ) returnval=false;                  // apply jet pT cut
    if ( fabs(jet.eta()) < jetAbsEtaMin_ || fabs(jet.eta()) > jetAbsEtaMax_ ) returnval=false; // apply jet eta cut


    // often we have way to many gluons that we do not need. This randomply reduces the gluons
    if (gluonReduction_>0 && jet.partonFlavour()==21)
      if(TRandom_.Uniform()>gluonReduction_) {
	returnval=false;
      }
	//std::cout<<"Jet removed for gluon reduction "<<returnval<<std::endl;}

    //branch fills
    /*for(auto& entry : discriminators_) {
        entry.second = catchInfs(jet.bDiscriminator(entry.first),-0.1);
    }*/

    npv_ = vertices()->size();

    for (auto const& v : *pupInfo()) {
        int bx = v.getBunchCrossing();
        if (bx == 0) {
            ntrueInt_ = v.getTrueNumInteractions();
        }
    }
    rho_ = rhoInfo()[0];

    jet_no_=jetidx;

    const auto jetRef = reco::CandidatePtr(coll->ptrs().at( jetidx));

    jet_qgl_ = (*qglHandle)[jetRef];
    QG_ptD_ = (*ptDHandle)[jetRef];
    QG_axis2_ = (*axis2Handle)[jetRef];
    QG_mult_ = (*multHandle)[jetRef];

    isB_=0; isGBB_=0; isBB_=0; isC_=0; isGCC_=0; isCC_=0; isU_=0; isD_=0; isMU_=0; isELE_=0;
    isS_=0; isG_=0, isPU_=0, isLeptonicB_=0, isLeptonicB_C_=0, isUndefined_=0;
    isTaup1h0p_=0, isTaup1h1p_=0, isTaup1h2p_=0, isTaup3h0p_=0, isTaup3h1p_=0; isTaupOther_=0;
    isTaum1h0p_=0, isTaum1h1p_=0, isTaum1h2p_=0, isTaum3h0p_=0, isTaum3h1p_=0; isTaumOther_=0;isDiTauh_=0; isDiTaumu_=0; isDiTaue_=0;
    auto muIds = deep_ntuples::jet_muonsIds(jet,*muonsHandle);
    auto elecIds = deep_ntuples::jet_electronsIds(jet,*electronsHandle);

    muons_number_ = muIds.size();
    electrons_number_ = elecIds.size();

    float etasign = 1.;
    if (jet.eta()<0) etasign = -1.;

    for(std::size_t i=0; i<max_num_lept; i++) {
        if (i < muIds.size()) {
            const auto & muon = (*muonsHandle).at(muIds.at(i));
            muons_isLooseMuon_[i] = muon.isLooseMuon();
            muons_isTightMuon_[i] = muon.isTightMuon(vertices()->at(0));
            muons_isSoftMuon_[i] = muon.isSoftMuon(vertices()->at(0));
            muons_isHighPtMuon_[i] = muon.isHighPtMuon(vertices()->at(0));
            muons_pt_[i] = muon.pt();
            muons_relEta_[i] = etasign*(muon.eta()-jet.eta());
            muons_relPhi_[i] = reco::deltaPhi(muon.phi(),jet.phi());
            muons_energy_[i] = muon.energy()/jet.energy();
        }
        if (i < elecIds.size()) {
            const auto & electron = (*electronsHandle).at(elecIds.at(i));
            electrons_pt_[i] = electron.pt();
            electrons_relEta_[i] = etasign*(electron.eta()-jet.eta());
            electrons_relPhi_[i] = reco::deltaPhi(electron.phi(),jet.phi());
            electrons_energy_[i] = electron.energy()/jet.energy();
        }
    }
    gen_number_ = std::min(gen_particle_pt.size(),max_num_gen_);
    gend_number_ = std::min(gen_particle_daughters_pt.size(),max_num_gen_);
    for(size_t i=0; i< (size_t)gen_number_; i++) {
      gen_particle_pt_[i] = gen_particle_pt.at(i);
      gen_particle_eta_[i] = gen_particle_eta.at(i);
      gen_particle_phi_[i] = gen_particle_phi.at(i);
      gen_particle_mass_[i] = gen_particle_mass.at(i);
      gen_particle_id_[i] = gen_particle_id.at(i);
      gen_particle_status_[i] = gen_particle_status.at(i);
    }
    for(size_t i=0; i< (size_t)gend_number_; i++) {
      gen_particle_daughters_id_[i] = gen_particle_daughters_id.at(i);
      gen_particle_daughters_igen_[i] = gen_particle_daughters_igen.at(i);
      gen_particle_daughters_pt_[i] = gen_particle_daughters_pt.at(i);
      gen_particle_daughters_eta_[i] = gen_particle_daughters_eta.at(i);
      gen_particle_daughters_phi_[i] = gen_particle_daughters_phi.at(i);
      gen_particle_daughters_mass_[i] = gen_particle_daughters_mass.at(i);
      gen_particle_daughters_status_[i] = gen_particle_daughters_status.at(i);
      gen_particle_daughters_charge_[i] = gen_particle_daughters_charge.at(i);
    }

    if (!KeepSingleTau_) {
      pos_matched_tauh = -1;
    }
    //// Note that jets with gluon->bb (cc) and x->bb (cc) are in the same categories
    if(true){
      switch(deep_ntuples::jet_flavour(jet, gToBB, gToCC, neutrinosLepB, neutrinosLepB_C, alltaus_, pos_matched_genmu, pos_matched_genele, pos_matched_tauh, pos_matched_tauhtauh, pos_matched_tauhtaue, pos_matched_tauhtaumu, gentau_decaymode, tau_gen_charge)) {
        case deep_ntuples::JetFlavor::MU:  isMU_=1; break;
        case deep_ntuples::JetFlavor::ELE:  isELE_=1; break;
        case deep_ntuples::JetFlavor::TAUP1H0P:  isTaup1h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUP1H1P:  isTaup1h1p_=1; break;
        case deep_ntuples::JetFlavor::TAUP1H2P:  isTaup1h2p_=1; break;
        case deep_ntuples::JetFlavor::TAUP3H0P:  isTaup3h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUP3H1P:  isTaup3h1p_=1; break;
        case deep_ntuples::JetFlavor::TAUM1H0P:  isTaum1h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUM1H1P:  isTaum1h1p_=1; break;
        case deep_ntuples::JetFlavor::TAUM1H2P:  isTaum1h2p_=1; break;
        case deep_ntuples::JetFlavor::TAUM3H0P:  isTaum3h0p_=1; break;
        case deep_ntuples::JetFlavor::TAUM3H1P:  isTaum3h1p_=1; break;
        case deep_ntuples::JetFlavor::DITAUH:  isDiTauh_=1; break;
        case deep_ntuples::JetFlavor::DITAUMU:  isDiTaumu_=1; break;
        case deep_ntuples::JetFlavor::DITAUE:  isDiTaue_=1; break;
        case deep_ntuples::JetFlavor::B:  isB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB: isLeptonicB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB_C: isLeptonicB_C_=1; break;
        case deep_ntuples::JetFlavor::GBB: isGBB_=1; break;
        case deep_ntuples::JetFlavor::BB: isBB_=1; break;
        case deep_ntuples::JetFlavor::C:  isC_=1; break;
        case deep_ntuples::JetFlavor::GCC: isGCC_=1; break;
        case deep_ntuples::JetFlavor::CC: isCC_=1; break;
        case deep_ntuples::JetFlavor::G:  isG_=1; break;
        case deep_ntuples::JetFlavor::PU:  isPU_=1; break;
        case deep_ntuples::JetFlavor::S:  isS_=1; break;
        case deep_ntuples::JetFlavor::U: isU_=1; break;
        case deep_ntuples::JetFlavor::D: isD_=1; break;
        default : isUndefined_=1; break;
        }

      if (isTaup1h0p_ || isTaup1h1p_ || isTaup1h2p_ || isTaup3h0p_ || isTaup3h1p_ ||
	  isTaum1h0p_ || isTaum1h1p_ || isTaum1h2p_ || isTaum3h0p_ || isTaum3h1p_) {
	if(debug){std::cout << "        !!!!!!!!! This jet is a single-tau!" << std::endl;}
      }

      if(debug){std::cout << "  isTaup1h0p_: " << isTaup1h0p_ << ", isTaup1h1p_: " << isTaup1h1p_
            << ", isTaup1h2p_: " << isTaup1h2p_ << ", isTaup3h0p_: " << isTaup3h0p_
			  << ", isTaup3h1p_: " << isTaup3h1p_ << std::endl;}
      if(debug){std::cout << "  isTaum1h0p_: " << isTaum1h0p_ << ", isTaum1h1p_: " << isTaum1h1p_
		<< ", isTaum1h2p_: " << isTaum1h2p_ << ", isTaum3h0p_: " << isTaum3h0p_
			  << ", isTaum3h1p_: " << isTaum3h1p_ << std::endl;}
      if (isDiTauh_ || isDiTaumu_ || isDiTaue_) {
	if(debug){std::cout << "        !!!!!!!!! This jet is a di-tau!  " <<std::endl;}
      }
      if(debug){std::cout << "  isDiTauh_: " << isDiTauh_ << ", isDiTaumu_: " << isDiTaumu_
			  << ", isDiTaue_: " << isDiTaue_ << std::endl;}

    }
    if (SkipPU_ && isPU_) returnval=false; 
    if (KeepOnlyB_ && !isB_ && !isBB_ && !isGBB_ && !isLeptonicB_ && isLeptonicB_C_) 
        returnval=false;
    if (KeepOnlyBC_ && !isB_ && !isBB_ && !isGBB_ && !isLeptonicB_ && !isLeptonicB_C_ && !isC_ && !isCC_ && !isGCC_)
        returnval=false;
    if (KeepOnlyUDS_ && !isU_ && !isD_ && !isS_)
        returnval=false;       
    if (KeepOnlyG_ && !isG_)
        returnval=false;
    if (KeepOnlyPU_ && !isPU_)
        returnval=false;

    //truth labeling with fallback to physics definition for light/gluon/undefined of standard flavor definition
    //// Note that jets with gluon->bb (cc) and x->bb (cc) are in the same categories
    isPhysB_=0; isPhysBB_=0; isPhysGBB_=0; isPhysC_=0; isPhysCC_=0;
    isPhysGCC_=0; isPhysD_=0; isPhysU_=0; isPhysS_=0; isPhysG_=0, isPhysLeptonicB_=0, isPhysLeptonicB_C_=0, isPhysUndefined_=0;
    isPhysTau_=0, isPhysPU_=0;
    if(true){
      switch(deep_ntuples::jet_flavour(jet, gToBB, gToCC, neutrinosLepB, neutrinosLepB_C, alltaus_, pos_matched_genmu, pos_matched_genele, pos_matched_tauh, pos_matched_tauhtauh, pos_matched_tauhtaue, pos_matched_tauhtaumu, gentau_decaymode, tau_gen_charge, true)) {
        case deep_ntuples::JetFlavor::S:  isPhysS_=1; break;
        case deep_ntuples::JetFlavor::U: isPhysU_=1; break;
        case deep_ntuples::JetFlavor::D: isPhysD_=1; break;
        case deep_ntuples::JetFlavor::B:  isPhysB_=1; break;
        case deep_ntuples::JetFlavor::BB: isPhysBB_=1; break;
        case deep_ntuples::JetFlavor::GBB: isPhysGBB_=1; break;
        case deep_ntuples::JetFlavor::C:  isPhysC_=1; break;
        case deep_ntuples::JetFlavor::CC: isPhysCC_=1; break;
        case deep_ntuples::JetFlavor::GCC: isPhysGCC_=1; break;
        case deep_ntuples::JetFlavor::TAU: isPhysTau_=1;break;
        case deep_ntuples::JetFlavor::G:  isPhysG_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB: isPhysLeptonicB_=1; break;
        case deep_ntuples::JetFlavor::LeptonicB_C: isPhysLeptonicB_C_=1; break;
	case deep_ntuples::JetFlavor::PU: isPhysPU_=1; break;
        default : isPhysUndefined_=1; break;
        }
    }

    //2nd matching
    constituent_pt_.clear();
    constituent_eta_.clear();
    constituent_phi_.clear();
    constituent_pdgId_.clear();
    constituent_BdaughterDr_.clear();
    constituent_BdaughterIdx_.clear();
    constituent_CdaughterDr_.clear();
    constituent_CdaughterIdx_.clear();

    std::vector<int> matchedB;
    nMatchedDaughtersB_=0;
    nMatchedDaughtersC_=0;
    nConstituent_=jet.numberOfDaughters();
    for (unsigned int ireco=0; ireco< jet.numberOfDaughters(); ireco++){
       float minDR=100;
       float minIdx=-1;
       for (unsigned int igen=0; igen< genBmeson_daughter_pdgId_.size(); igen++){
         float tempDR=deltaR(genBmeson_daughter_eta_[igen],genBmeson_daughter_phi_[igen],jet.daughter(ireco)->eta(),jet.daughter(ireco)->phi());
         if (minDR<tempDR ) 
            continue;
         minDR = tempDR;
         minIdx = igen;
       }
       constituent_pt_.push_back(jet.daughter(ireco)->pt());
       constituent_eta_.push_back(jet.daughter(ireco)->eta());
       constituent_phi_.push_back(jet.daughter(ireco)->phi());
       constituent_pdgId_.push_back(jet.daughter(ireco)->pdgId());
       if (minDR<0.05){
         nMatchedDaughtersB_+=1;
         if (std::find(matchedB.begin(),matchedB.end(),minIdx) == matchedB.end())
            matchedB.push_back(minIdx);
         constituent_BdaughterDr_.push_back(minDR);
         constituent_BdaughterIdx_.push_back(minIdx);
         constituent_CdaughterDr_.push_back(10);
         constituent_CdaughterIdx_.push_back(-1);
       } else{
         constituent_BdaughterDr_.push_back(10);
         constituent_BdaughterIdx_.push_back(-1);
         minDR=100;
         minIdx=-1;
         for (unsigned int igen=0; igen< genCmeson_daughter_pdgId_.size(); igen++){
            float tempDR=deltaR(genCmeson_daughter_eta_[igen],genCmeson_daughter_phi_[igen],jet.daughter(ireco)->eta(),jet.daughter(ireco)->phi());
            if (minDR<tempDR )
               continue;
            minDR = tempDR;
            minIdx = igen;
         }
         if (minDR<0.05){
            nMatchedDaughtersC_+=1;
            constituent_CdaughterDr_.push_back(minDR);
            constituent_CdaughterIdx_.push_back(minIdx);
         } else{
            constituent_CdaughterDr_.push_back(10);
            constituent_CdaughterIdx_.push_back(-1);
         }
       }
    }

    isMatchedBB_=0;
    isMatchedB_=0;
    isNotMatched_=0;
    isMatchedC_=0;    
    isMatchedBAndC_=0;
    if (nMatchedDaughtersB_>0 and nMatchedDaughtersC_==0 and matchedB.size()>1) isMatchedBB_=1;
    if (nMatchedDaughtersB_>0 and nMatchedDaughtersC_==0 and matchedB.size()==1) isMatchedB_=1; 
    if (nMatchedDaughtersB_==0 and nMatchedDaughtersC_>0) isMatchedC_=1;
    if (nMatchedDaughtersB_>0 and nMatchedDaughtersC_>0) isMatchedBAndC_=1;
    if (nMatchedDaughtersB_==0 and nMatchedDaughtersC_==0) isNotMatched_=1;

    isMatchedB1d_=0;
    isMatchedB2d_=0;
    isMatchedB3d_=0;
    isMatchedBMore3d_=0;
    if (matchedB.size()==1 and isMatchedBAndC_==0){
       if (nMatchedDaughtersB_==1) isMatchedB1d_=1;
       else if (nMatchedDaughtersB_==2) isMatchedB2d_=1;
       else if (nMatchedDaughtersB_==3) isMatchedB3d_=1;
       else if (nMatchedDaughtersB_>3) isMatchedBMore3d_=1;
    }
    
    isMatchedBB2d_=0;
    isMatchedBB3d_=0;
    isMatchedBB4d_=0;
    isMatchedBBMore4d_=0;
    if (matchedB.size()>1  and isMatchedBAndC_==0){
       if (nMatchedDaughtersB_==2) isMatchedBB2d_=1;
       else if (nMatchedDaughtersB_==3) isMatchedBB3d_=1;
       else if (nMatchedDaughtersB_==4) isMatchedBB4d_=1;
       else if (nMatchedDaughtersB_>4) isMatchedBBMore4d_=1;
    }

    isMatchedC1d_=0;
    isMatchedC2d_=0;
    isMatchedC3d_=0;
    isMatchedCMore3d_=0;
    if (nMatchedDaughtersC_==1 and isMatchedBAndC_==0) isMatchedC1d_=1;
    else if (nMatchedDaughtersC_==2 and isMatchedBAndC_==0) isMatchedC2d_=1;
    else if (nMatchedDaughtersC_==3 and isMatchedBAndC_==0) isMatchedC3d_=1;
    else if (nMatchedDaughtersC_>3 and isMatchedBAndC_==0) isMatchedCMore3d_=1;
   

    isMatchedB1dC1d_=0;
    isMatchedB2dC1d_=0;
    isMatchedBMore2dC1d_=0;
    isMatchedB1dC2d_=0;
    isMatchedB2dC2d_=0;
    isMatchedBMore2dC2d_=0;
    isMatchedB1dCMore2d_=0;
    isMatchedB2dCMore2d_=0;
    isMatchedBMore2dCMore2d_=0;    
    if ( isMatchedBAndC_>0){
      if (nMatchedDaughtersB_==1 and nMatchedDaughtersC_==1) isMatchedB1dC1d_=1;
      else if (nMatchedDaughtersB_==2 and nMatchedDaughtersC_==1) isMatchedB2dC1d_=1;
      else if (nMatchedDaughtersB_>2 and nMatchedDaughtersC_==1) isMatchedBMore2dC1d_=1;
      else if (nMatchedDaughtersB_==1 and nMatchedDaughtersC_==2) isMatchedB1dC2d_=1;
      else if (nMatchedDaughtersB_==2 and nMatchedDaughtersC_==2) isMatchedB2dC2d_=1;
      else if (nMatchedDaughtersB_>2 and nMatchedDaughtersC_==2) isMatchedBMore2dC2d_=1;
      else if (nMatchedDaughtersB_==1 and nMatchedDaughtersC_>2) isMatchedB1dCMore2d_=1;
      else if (nMatchedDaughtersB_==2 and nMatchedDaughtersC_>2) isMatchedB2dCMore2d_=1;
      else if (nMatchedDaughtersB_>2 and nMatchedDaughtersC_>2) isMatchedBMore2dCMore2d_=1;
    }



    if(isUndefined_) {
      returnval=false;
      //std::cout<<"jet is undefined "<<returnval<<std::endl;}
    }
    pat::JetCollection h;

    jet_pt_ = jet.pt();
    jet_eta_ = jet.eta();
    jet_phi_ = jet.phi();
    jet_corr_pt_ = jet.pt();
    jet_mass_ = jet.mass();
    jet_energy_ = jet.energy();
    jet_deepjet_probb_ = jet.bDiscriminator("pfDeepFlavourJetTags:probb") + jet.bDiscriminator("pfDeepFlavourJetTags:probbb") + jet.bDiscriminator("pfDeepFlavourJetTags:problepb");
    // Matching with gen-jets                                                                                                                                                                               
    int genjet_pos_matched = -1;
    float gen_minDR = dRCone;
    for(size_t igen = 0; igen < jetv_gen.size(); igen++){
      if(reco::deltaR(jetv_gen[igen]->p4(),jet.p4()) < gen_minDR){
        genjet_pos_matched = igen;
        gen_minDR = reco::deltaR(jetv_gen[igen]->p4(),jet.p4());
      }
    }

    int genjet_wnu_pos_matched = -1;
    float gen_minDR_wnu = dRCone;
    for(size_t igen = 0; igen < jetv_gen_wnu.size(); igen++){
      if(reco::deltaR(jetv_gen_wnu[igen]->p4(),jet.p4()) < gen_minDR_wnu){
        genjet_wnu_pos_matched = igen;
        gen_minDR_wnu = reco::deltaR(jetv_gen_wnu[igen]->p4(),jet.p4());
      }
    }

    jet_genmatch_pt_ = -1.0;
    jet_genmatch_eta_ = -1.0;
    jet_genmatch_phi_ = -1.0;
    jet_genmatch_wnu_pt_ = -1.0;
    jet_genmatch_wnu_eta_ = -1.0;
    jet_genmatch_wnu_phi_ = -1.0; 

    if(genjet_pos_matched >= 0){
      jet_genmatch_pt_ = jetv_gen[genjet_pos_matched]->pt();
      jet_genmatch_eta_ = jetv_gen[genjet_pos_matched]->eta();
      jet_genmatch_phi_ = jetv_gen[genjet_pos_matched]->phi();
    }

    if(genjet_wnu_pos_matched >= 0){
      jet_genmatch_wnu_pt_ = jetv_gen_wnu[genjet_wnu_pos_matched]->pt();
      jet_genmatch_wnu_eta_ = jetv_gen_wnu[genjet_wnu_pos_matched]->eta();
      jet_genmatch_wnu_phi_ = jetv_gen_wnu[genjet_wnu_pos_matched]->phi();
    }

    genDecay_ = -1.;
    
    try {
        reco::GenParticleRefVector Bhadrons_in_jet = jet.jetFlavourInfo().getbHadrons();

        if (Bhadrons_in_jet.size() > 0){ 

            for (unsigned int idx=0; idx<Bhadron_.size(); ++idx){

                reco::GenParticle bhad = Bhadron_[idx];

                bool bhad_is_in_jet = false;

                for (reco::GenParticleRefVector::const_iterator bhad_in_jet = Bhadrons_in_jet.begin(); bhad_in_jet!=Bhadrons_in_jet.end(); ++bhad_in_jet) {

                    //check if bhad is identical to bhad_in_jet
                    if ( (*bhad_in_jet)->pt() == bhad.pt() && (*bhad_in_jet)->eta() == bhad.eta()
                            && (*bhad_in_jet)->phi() == bhad.phi() && (*bhad_in_jet)->pdgId() == bhad.pdgId())              
                        bhad_is_in_jet = true;
                }
                if (bhad_is_in_jet){

                    if (Bhadron_daughter_[idx].vx()!=bhad.vx()){

                        float vx = Bhadron_daughter_[idx].vx() - bhad.vx();
                        float vy = Bhadron_daughter_[idx].vy() - bhad.vy();

                        float dxy = sqrt(vx*vx+vy*vy);
                        if (dxy > genDecay_)
                            genDecay_= dxy;
                    }
                    else if (genDecay_ < 0) 
                        genDecay_ = -0.1;
                }
            }
        }
    }
    catch (const cms::Exception &e){
        genDecay_ = -1.;
    }

    try{
        float NHF  = jet.neutralHadronEnergyFraction();
        float NEMF = jet.neutralEmEnergyFraction();
        float CHF  = jet.chargedHadronEnergyFraction();
        float MUF  = jet.muonEnergyFraction();
        float CEMF = jet.chargedEmEnergyFraction();
        float NumConst = jet.chargedMultiplicity()+jet.neutralMultiplicity();
        float NumNeutralParticles =jet.neutralMultiplicity();
        float CHM      = jet.chargedMultiplicity();

        jet_looseId_ = ((NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7) ||
                (NHF<0.98 && NEMF>0.01 && NumNeutralParticles>2 && abs(jet_eta_)>2.7 && abs(jet_eta_)<=3.0 ) ||
                (NEMF<0.90 && NumNeutralParticles>10 && abs(jet_eta_)>3.0 );
        //I do it only for eta that I use
        bool tightJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7;
        bool tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((abs(jet_eta_)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || abs(jet_eta_)>2.4) && abs(jet_eta_)<=2.7;
        jet_jetId_=tightJetID*2+4*tightLepVetoJetID;
        
    }catch(const cms::Exception &e){
        jet_looseId_ = -1;
        jet_jetId_=-1;
    }
    
    jet_puId_= 0;
    jet_hflav_=abs(jet.hadronFlavour());
    jet_pflav_=abs(jet.partonFlavour());
    jet_phflav_=0;
    if(jet.genParton()) jet_phflav_=abs(jet.genParton()->pdgId());

    gen_pt_ =  0;
    Delta_gen_pt_ =  0;
    gen_pt_Recluster_=0;
    gen_pt_WithNu_=0;
    Delta_gen_pt_Recluster_=0;
    Delta_gen_pt_WithNu_=0;
    if(!jet.genJet()){
        const edm::RefToBase<pat::Jet> patJetRef = coll->refAt(jetidx);
	reco::GenJetRef genjetDuplication = (*genJetMatchAllowDuplicates)[patJetRef];
        if (genjetDuplication.isNonnull() && genjetDuplication.isAvailable()) {
	  returnval=false;
	  //std::cout<<"Jet duplication "<<returnval<<std::endl;
        }

    }

    if(jet.genJet()){
        gen_pt_ =  jet.genJet()->pt();
        Delta_gen_pt_ =  jet.genJet()->pt()- jet_pt_;

	float deltaR_match = reco::deltaR(jet, *jet.genJet());
	if( deltaR_match > 0.2){
	  returnval = false;
	  //std::cout<<"Jet gen match "<<returnval<<std::endl;
	}

        const edm::RefToBase<pat::Jet> patJetRef = coll->refAt(jetidx);
        reco::GenJetRef genjetRecluster = (*genJetMatchRecluster)[patJetRef];

        gen_pt_Recluster_ = 0.;
        if (genjetRecluster.isNonnull() && genjetRecluster.isAvailable()) {
            gen_pt_Recluster_ = genjetRecluster->pt();
        }
        reco::GenJetRef genjetWithNu = (*genJetMatchWithNu)[patJetRef];

        gen_pt_WithNu_ = 0.;
        if (genjetWithNu.isNonnull() && genjetWithNu.isAvailable()) {
            gen_pt_WithNu_ = genjetWithNu->pt();
        }

        Delta_gen_pt_Recluster_=gen_pt_Recluster_-jet.pt();
        Delta_gen_pt_WithNu_=gen_pt_WithNu_-jet.pt();
    }

    auto qgtuple=yuta::calcVariables(&jet);
    //(multiplicity, charged_multiplicity, neutral_multiplicity, ptD, axis1, axis2, pt_dr_log);

    y_multiplicity_=std::get<0>(qgtuple);
    y_charged_multiplicity_=std::get<1>(qgtuple);
    y_neutral_multiplicity_=std::get<2>(qgtuple);
    y_ptD_    =  std::get<3>(qgtuple);
    y_axis1_  =  std::get<4>(qgtuple);
    y_axis2_  =  std::get<5>(qgtuple);
    y_pt_dr_log_=std::get<6>(qgtuple);

    if(debug){std::cout<<"Are we saving the jet? "<<returnval<<std::endl;}

    /*
    if (returnval) {
    *used_gentaus_ = tmp_used_gentaus_;  // update the real set
    // copy other tmp info to final storage
    }*/
    return returnval;
}
