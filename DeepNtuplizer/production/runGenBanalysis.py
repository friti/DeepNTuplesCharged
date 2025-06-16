
import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
### parsing job options 
import sys

options = VarParsing.VarParsing()

options.register('inputScript','',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register('outputFile','rungen_output2',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register('maxEvents', 10001,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('job', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "job number")
options.register('nJobs', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "total jobs")
options.register('reportEvery', 1000, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "report every")
options.register('gluonReduction', 0.0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.float, "gluon reduction")
options.register('selectJets', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "select jets with good gen match")
options.register('phase2', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "apply jet selection for phase 2. Currently sets JetEtaMax to 3.0 and picks slimmedJetsPuppi as jet collection.")
options.register('puppi', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "use puppi jets")
options.register('eta', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "use eta up to 5.0")


import os
release=os.environ['CMSSW_VERSION'][6:]
print("Using release "+release)


inputFiles =[ 'file:/eos/cms/store/cmst3/group/softJets/gkaratha/chain_m70_dm20_cfgRun24_133X_Run2024_test_10172024/Mini/chain_m70_dm20_'+str(i)+'_step5_mini.root' for i in range(1,250)]

#inputFiles = ['/store/mc/RunIII2024Summer24MiniAOD/TTto4Q_TuneCP5_13p6TeV_powheg-pythia8/MINIAODSIM/140X_mcRun3_2024_realistic_v26-v2/100000/001b1bf1-e811-4ec1-94ff-fa8a45fb629f.root']

if hasattr(sys, "argv"):
    options.parseArguments()


UsePuppiForTrkJet=False
UsePFForTrkJet=False
UseCHSForTrkJet=True

UsePuppiReclusterForStdJet=False
UsePFReclusterForStdJet=False
UseSlimmedForStdJet=False
UseCHSReclusterForStdJet=True



if (not UsePuppiReclusterForStdJet) and (not UsePFReclusterForStdJet) and (not UseSlimmedForStdJet) and (not UseCHSReclusterForStdJet):
   print("provide std jet")
   exit()

if (UsePuppiReclusterForStdJet + UsePFReclusterForStdJet + UseSlimmedForStdJet + UseCHSReclusterForStdJet)>1:
   print("too many std jet")
   exit()

if (not UsePuppiForTrkJet) and (not UsePFForTrkJet) and (not UseCHSForTrkJet):
   print("provide track jet")
   exit()

if (UsePFForTrkJet + UsePuppiForTrkJet + UseCHSForTrkJet)>1:
   print("too many track jet")
   exit()

process = cms.Process("DNNFiller")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '130X_mcRun3_2023_realistic_postBPix_v2', '')


process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(False)
)


process.source = cms.Source ("PoolSource",fileNames = cms.untracked.vstring(inputFiles), secondaryFileNames = cms.untracked.vstring())
process.source.skipEvents = cms.untracked.uint32(options.skipEvents)
process.maxEvents  = cms.untracked.PSet( 
    input = cms.untracked.int32 (options.maxEvents) 
)
releases = release.split("_")


from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
    
###########################################################################
#
# Setup puppi modules and set them to recalculate weights
#
###########################################################################
from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD
makePuppiesFromMiniAOD(process, False)
process.puppi.useExistingWeights = False
process.puppiNoLep.useExistingWeights = False

from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask, addToProcessAndTask
from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection


###########################################################################
#
# Make function wrapper around PatAlgos helper functions
#
###########################################################################
def addProcessAndTask(proc, label, module):
  task = getPatAlgosToolsTask(proc)
  addToProcessAndTask(label, module, proc, task)

from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJetsPuppi
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJetsCHS



###############################################################################
####################### Reclustered Standard AK4 jets ##########################
###############################################################################

############################## Puppi recluster jets ###########################
jetCorrectionsAK4 = ('AK4PFPuppi', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
if UsePuppiReclusterForStdJet:
  addProcessAndTask(process, "ak4PFJetsPuppiRecluster", ak4PFJetsPuppi.clone(
        src = "packedPFCandidates",
        srcWeights = "puppi",
        doAreaFastjet = True,
        jetPtMin=5
     )
  )

  addJetCollection(
      process,
      postfix            = "Recluster",
      labelName          = "AK4Puppi",
      jetSource          = cms.InputTag("ak4PFJetsPuppiRecluster"),
      algo               = "AK", #name of algo must be in this format
      rParam             = 0.4,
      pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidates       = cms.InputTag("packedPFCandidates"),
      svSource           = cms.InputTag("slimmedSecondaryVertices"),
      muSource           = cms.InputTag("slimmedMuons"),
      elSource           = cms.InputTag("slimmedElectrons"),
      genJetCollection   = cms.InputTag("ak4GenJetsRecluster"), # This is setup below
      genParticles       = cms.InputTag("prunedGenParticles"),
      jetCorrections     = jetCorrectionsAK4,
  )
  process.patJetsAK4PuppiRecluster.getJetMCFlavour = True
  getattr(process, "patJetFlavourAssociationAK4PuppiRecluster").weights = cms.InputTag("puppi")
  #process.selectedPatJetsAK4PuppiRecluster.cut="pt > 10"
  updateJetCollection(
        process,
        labelName = "AK4PuppiR",
        jetSource = cms.InputTag("selectedPatJetsAK4PuppiRecluster"),  # 'ak4Jets'
        jetCorrections = jetCorrectionsAK4,
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        btagInfos = None,
        btagDiscriminators = None,
        explicitJTA = False
  )


################################# CHS recluster jets ########################
if UseCHSReclusterForStdJet:
   from CommonTools.ParticleFlow.pfNoPileUpJME_cff import primaryVertexAssociationJME

   addProcessAndTask(process, "chsPFCandidates",cms.EDFilter("CandPtrSelector",
     src = cms.InputTag("packedPFCandidates"),
     cut = cms.string("fromPV(0)>0 || (vertexRef().key<={} && abs(dz(0))<{})".format(
                  primaryVertexAssociationJME.assignment.NumOfPUVtxsForCharged.value(),
                  primaryVertexAssociationJME.assignment.DzCutForChargedFromPUVtxs.value()))
     )
)


   addProcessAndTask(process, "ak4PFJetsChsRecluster", ak4PFJets.clone(
        src = "chsPFCandidates",
        doAreaFastjet = True,
        jetPtMin=5
     )
   )

   addJetCollection(
      process,
      postfix            = "Recluster",
      labelName          = "AK4Chs",
      jetSource          = cms.InputTag("ak4PFJetsChsRecluster"),
      algo               = "AK", #name of algo must be in this format
      rParam             = 0.4,
      pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidates       = cms.InputTag("packedPFCandidates"),
      svSource           = cms.InputTag("slimmedSecondaryVertices"),
      muSource           = cms.InputTag("slimmedMuons"),
      elSource           = cms.InputTag("slimmedElectrons"),
      genJetCollection   = cms.InputTag("ak4GenJetsRecluster"), # This is setup below
      genParticles       = cms.InputTag("prunedGenParticles"),
      jetCorrections     = jetCorrectionsAK4,
   )
   updateJetCollection(
        process,
        labelName = "AK4ChsR",
        jetSource = cms.InputTag("selectedPatJetsAK4ChsRecluster"),  # 'ak4Jets'
        jetCorrections = jetCorrectionsAK4,
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        btagInfos = None,
        btagDiscriminators = None,
        explicitJTA = False
    )



################################# PF recluster jets #########################
if UsePFReclusterForStdJet:
  addProcessAndTask(process, "ak4PFReclusterJets", ak4PFJets.clone(
          src = "packedPFCandidates",
          jetPtMin=5,
          doAreaFastjet = True,
        )
  )
  
  addJetCollection(
        process,
        labelName          = "AK4PFRecluster",
        jetSource          = cms.InputTag("ak4PFReclusterJets"),
        algo               = "ak", #name of algo must be in this format
        rParam             = 0.4,
        pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
        pfCandidates       = cms.InputTag("packedPFCandidates"),
        svSource           = cms.InputTag("slimmedSecondaryVertices"),
        muSource           = cms.InputTag("slimmedMuons"),
        elSource           = cms.InputTag("slimmedElectrons"),
        genJetCollection   = cms.InputTag("ak4GenJetsRecluster"),
        genParticles       = cms.InputTag("prunedGenParticles"),
        jetCorrections     = None ,
  )
  
  updateJetCollection(
        process,
        labelName = "AK4PFR",
        jetSource = cms.InputTag("patJetsAK4PFRecluster"),
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        jetCorrections = None,
        btagDiscriminators = None,
        btagInfos = None,
        explicitJTA = False
  )


#############################################################################
############################## trackjet #####################################
#############################################################################
#### filter charged PF cands
addProcessAndTask(process, "packedPFCandidatesChg",cms.EDFilter("CandPtrSelector",
     src = cms.InputTag("packedPFCandidates"),
     cut = cms.string("charge != 0 && pvAssociationQuality>3")
     )
)
#pvAssociationQuality=6 fitloose
#pvAssociationQuality=7 fittight
#pvAssociationQuality=4 btag

############################## PF trkjet ###################################
if UsePFForTrkJet:
   addProcessAndTask(process, "ak4PFChgJets", ak4PFJets.clone(
            src = "packedPFCandidatesChg",
            jetPtMin=5,
            doAreaFastjet = True
            )
   )

   addJetCollection(
      process,
      labelName          = "AK4PFChg",
      jetSource          = cms.InputTag("ak4PFChgJets"),
      algo               = "ak", #name of algo must be in this format
      rParam             = 0.4,
      pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidates       = cms.InputTag("packedPFCandidates"),
      svSource           = cms.InputTag("slimmedSecondaryVertices"),
      muSource           = cms.InputTag("slimmedMuons"),
      elSource           = cms.InputTag("slimmedElectrons"),
      genJetCollection   = cms.InputTag("ak4GenJetsRecluster"),
      genParticles       = cms.InputTag("prunedGenParticles"),
      jetCorrections     = None ,
   )

   updateJetCollection(
        process,
        labelName = "AK4PFChgR",
        jetSource = cms.InputTag("selectedPatJetsAK4PFChg"),
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        jetCorrections = None,
        btagDiscriminators = None,
        btagInfos = None,
        explicitJTA = False
   )

################################# Puppi trkjet ###############################
if UsePuppiForTrkJet:
  addProcessAndTask(process, "ak4PuppiChgJets", ak4PFJetsPuppi.clone(
        src = "packedPFCandidatesChg",
        srcWeights = "puppi",
        doAreaFastjet = True,
        jetPtMin=5
     )
  )

  addJetCollection(
      process,
      labelName          = "AK4PuppiChg",
      jetSource          = cms.InputTag("ak4PuppiChgJets"),
      algo               = "AK", #name of algo must be in this format
      rParam             = 0.4,
      pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidates       = cms.InputTag("packedPFCandidates"),
      svSource           = cms.InputTag("slimmedSecondaryVertices"),
      muSource           = cms.InputTag("slimmedMuons"),
      elSource           = cms.InputTag("slimmedElectrons"),
      genJetCollection   = cms.InputTag("ak4GenJetsRecluster"), # This is setup below
      genParticles       = cms.InputTag("prunedGenParticles"),
      jetCorrections     = jetCorrectionsAK4,
  )

  process.patJetsAK4PuppiChg.getJetMCFlavour = True
  getattr(process, "patJetFlavourAssociationAK4PuppiChg").weights = cms.InputTag("puppi")

  updateJetCollection(
        process,
        labelName = "AK4PuppiChgR",
        jetSource = cms.InputTag("selectedPatJetsAK4PuppiChg"),  # 'ak4Jets'
        jetCorrections = jetCorrectionsAK4,
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        btagInfos = None,
        btagDiscriminators = None,
        explicitJTA = False
  )

############################## CHS trkjet ###################################
if UseCHSForTrkJet:
   from CommonTools.ParticleFlow.pfNoPileUpJME_cff import primaryVertexAssociationJME

   addProcessAndTask(process, "chsPFCandidatesChg",cms.EDFilter("CandPtrSelector",
     src = cms.InputTag("packedPFCandidates"),
     cut = cms.string("charge != 0 && (fromPV(0)>0 || (vertexRef().key<={} && abs(dz(0))<{}))".format(
                  primaryVertexAssociationJME.assignment.NumOfPUVtxsForCharged.value(),
                  primaryVertexAssociationJME.assignment.DzCutForChargedFromPUVtxs.value()))
     )
)

   addProcessAndTask(process, "ak4ChsChgJets", ak4PFJets.clone(
            src = "chsPFCandidatesChg",
            jetPtMin=5,
            doAreaFastjet = True
            )
   )

   addJetCollection(
      process,
      labelName          = "AK4ChsChg",
      jetSource          = cms.InputTag("ak4ChsChgJets"),
      algo               = "ak", #name of algo must be in this format
      rParam             = 0.4,
      pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
      pfCandidates       = cms.InputTag("packedPFCandidates"),
      svSource           = cms.InputTag("slimmedSecondaryVertices"),
      muSource           = cms.InputTag("slimmedMuons"),
      elSource           = cms.InputTag("slimmedElectrons"),
      genJetCollection   = cms.InputTag("ak4GenJetsRecluster"),
      genParticles       = cms.InputTag("prunedGenParticles"),
      jetCorrections     = None ,
   )

   updateJetCollection(
        process,
        labelName = "AK4ChsChgR",
        jetSource = cms.InputTag("selectedPatJetsAK4ChsChg"),
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        jetCorrections = None,
        btagDiscriminators = None,
        btagInfos = None,
        explicitJTA = False
   )




if UsePFForTrkJet:
   options.outputFile+="PFCvs"
   trk_jet_collection = 'selectedUpdatedPatJetsAK4PFChgR'
if UsePuppiForTrkJet:
   options.outputFile+="PupCvs"
   trk_jet_collection = 'selectedUpdatedPatJetsAK4PuppiChgR'
if UseCHSForTrkJet:
   options.outputFile+="ChsCvs"
   trk_jet_collection = 'selectedUpdatedPatJetsAK4ChsChgR'

if UseSlimmedForStdJet:
   std_jet_collection = "slimmedJetsPuppi"
   options.outputFile+="Slimmed"
if UsePFReclusterForStdJet:
   std_jet_collection = "selectedUpdatedPatJetsAK4PFR"
   options.outputFile+="PFRecluster"
if UsePuppiReclusterForStdJet:
   std_jet_collection = "selectedUpdatedPatJetsAK4PuppiR"
   options.outputFile+="PuppiRecluster"
if UseCHSReclusterForStdJet:
   std_jet_collection = "selectedUpdatedPatJetsAK4ChsR"
   options.outputFile+="ChsRecluster"

from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
process.ak4GenJetsWithNu = ak4GenJets.clone(src ='packedGenParticles')

 ## Filter out neutrinos from packed GenParticles
process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedGenParticles"), cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16 && charge!=0"))
 ## Define GenJets
process.ak4GenJetsRecluster = ak4GenJets.clone(src = 'packedGenParticlesForJetsNoNu')


process.genJetReclusterTask = cms.Task(process.packedGenParticlesForJetsNoNu,process.ak4GenJetsWithNu,process.ak4GenJetsRecluster)

# Very Loose IVF SV collection
from PhysicsTools.PatAlgos.tools.helpers import loadWithPrefix
loadWithPrefix(process, 'RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff', "looseIVF")
process.looseIVFinclusiveCandidateVertexFinder.primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices")
process.looseIVFinclusiveCandidateVertexFinder.tracks = cms.InputTag("packedPFCandidates")
process.looseIVFinclusiveCandidateVertexFinder.vertexMinDLen2DSig = cms.double(0.)
process.looseIVFinclusiveCandidateVertexFinder.vertexMinDLenSig = cms.double(0.)
process.looseIVFinclusiveCandidateVertexFinder.fitterSigmacut = 20

process.looseIVFcandidateVertexArbitrator.primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices")
process.looseIVFcandidateVertexArbitrator.tracks = cms.InputTag("packedPFCandidates")
process.looseIVFcandidateVertexArbitrator.secondaryVertices = cms.InputTag("looseIVFcandidateVertexMerger")
process.looseIVFcandidateVertexArbitrator.fitterSigmacut = 20


outFileName = options.outputFile + '_' + str(options.job) +  '.root'
print ('Using output file ' + outFileName)

process.TFileService = cms.Service("TFileService", 
                                   fileName = cms.string(outFileName))

# GenBanalyzer
process.load("DeepNTuples.DeepNtuplizer.GenBanalysis_cfi")
process.genbanalizer.pfChargedJets = cms.InputTag(trk_jet_collection)
process.genbanalizer.jets = cms.InputTag(std_jet_collection)
process.genbanalizer.writeJetPart = cms.bool(True)
process.genbanalizer.writeChgJetPart = cms.bool(True)
process.genbanalizer.writePFcands = cms.bool(False)


#1631
process.ProfilerService = cms.Service (
      "ProfilerService",
       firstEvent = cms.untracked.int32(1631),
       lastEvent = cms.untracked.int32(1641),
       paths = cms.untracked.vstring('p') 
)

#Trick to make it work in 9_1_X
process.tsk = cms.Task()
for mod in process.producers_().values(): #.itervalues():
    process.tsk.add(mod)
for mod in process.filters_().values(): #.itervalues():
    process.tsk.add(mod)

process.patAlgosToolsTask = getPatAlgosToolsTask(process)

process.p = cms.Path(
    process.tsk,
    process.patAlgosToolsTask,
)
process.ep = cms.EndPath(process.genbanalizer)

