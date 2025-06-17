
import FWCore.ParameterSet.Config as cms

import FWCore.ParameterSet.VarParsing as VarParsing
### parsing job options 
import sys

options = VarParsing.VarParsing()

options.register('inputScript','',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"input Script")
options.register('outputFile','output',VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"output File (w/o .root)")
options.register('maxEvents', -1,VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.int,"maximum events")
options.register('skipEvents', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "skip N events")
options.register('job', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "job number")
options.register('nJobs', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "total jobs")
options.register('reportEvery', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "report every")
options.register('gluonReduction', 0.0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.float, "gluon reduction")
options.register('selectJets', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "select jets with good gen match")
options.register('puppi', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "use puppi jets")


import os
release=os.environ['CMSSW_VERSION'][6:]
print("Using release "+release)


options.register(
	'inputFiles','',
	VarParsing.VarParsing.multiplicity.list,
	VarParsing.VarParsing.varType.string,
	"input files (default is the tt RelVal)"
	)

if hasattr(sys, "argv"):
    options.parseArguments()

if options.puppi:
    usePuppi = True
    options.outputFile+="_puppi"
else:
    usePuppi = False 
    options.outputFile+="_pfc"

process = cms.Process("DNNFiller")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '130X_mcRun3_2023_realistic_postBPix_v2', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery

process.options = cms.untracked.PSet(
   allowUnscheduled = cms.untracked.bool(True),  
   wantSummary=cms.untracked.bool(False)
)

process.load('DeepNTuples.DeepNtuplizer.samples.TTJetsPhase1_cfg') #default input

if options.inputFiles:
	process.source.fileNames = options.inputFiles

if options.inputScript != '' and options.inputScript != 'DeepNTuples.DeepNtuplizer.samples.TTJetsPhase1_cfg':
    process.load(options.inputScript)

numberOfFiles = len(process.source.fileNames)
numberOfJobs = options.nJobs
jobNumber = options.job


process.source.fileNames = process.source.fileNames[jobNumber:numberOfFiles:numberOfJobs]
if options.nJobs > 1:
    print ("running over these files:")
    print (process.source.fileNames)

#process.source.fileNames =  cms.untracked.vstring([ 'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_0_step4.root','file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1000_step4.root','file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1001_step4.root', 'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1002_step4.root',]) 
process.source.fileNames =  cms.untracked.vstring([ 'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_0_step4.root']) 

'''
process.source.fileNames =  cms.untracked.vstring([ 'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_0_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1000_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1001_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1002_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1003_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1004_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1005_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1006_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1007_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1008_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1009_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_100_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1010_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1011_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1012_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1013_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1014_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1015_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1016_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1017_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1018_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1019_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_101_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1020_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1021_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1022_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1023_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1024_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1025_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1026_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1027_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1028_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1029_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_102_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1030_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1031_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1032_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1033_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1034_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1035_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1036_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1037_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1038_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1039_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_103_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1040_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1041_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1042_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1043_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1044_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1045_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1046_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1047_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1048_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1049_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_104_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1050_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1051_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1052_step4.root',
'file:/eos/cms/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/job_1053_step4.root',
]) 
#'file:/eos/cms/store/cmst3/group/softJets/gkaratha/chain_m70_dm20_cfgRun24_133X_Run2024_test_10172024/Mini/chain_m70_dm20_'+str(i)+'_step5_mini.root' for i in range(1,50)]

'''
process.source.skipEvents = cms.untracked.uint32(options.skipEvents)
process.maxEvents  = cms.untracked.PSet( 
    input = cms.untracked.int32 (options.maxEvents) 
)
releases = release.split("_")

bTagInfos = [ 'pfDeepFlavourTagInfos',
             'pfImpactParameterTagInfos',
             'pfInclusiveSecondaryVertexFinderTagInfos',
             'pfParticleNetAK4TagInfos',] #['pfParticleTransformerAK4TagInfos',]

from RecoBTag.ONNXRuntime.pfParticleNetAK4_cff import _pfParticleNetAK4JetTagsAll as pfParticleNetAK4JetTagsAll
from RecoBTag.ONNXRuntime.pfParticleNetFromMiniAODAK4_cff import _pfParticleNetFromMiniAODAK4PuppiCentralJetTagsProbs
from RecoBTag.ONNXRuntime.pfUnifiedParticleTransformerAK4_cff import _pfUnifiedParticleTransformerAK4JetTagsAll


if (int(releases[0])>8) or ( (int(releases[0])==8) and (int(releases[1]) >= 4) ) :
 bTagDiscriminators = [
     'pfDeepCSVJetTags:probudsg', #to be fixed with new names
     'pfDeepCSVJetTags:probb',
     'pfDeepCSVJetTags:probc',
     'pfDeepCSVJetTags:probbb',
     'pfDeepFlavourJetTags:probb',
     'pfDeepFlavourJetTags:probbb',
     'pfDeepFlavourJetTags:problepb',
     'pfDeepFlavourJetTags:probc',
     'pfDeepFlavourJetTags:probuds',
     'pfDeepFlavourJetTags:probg',
     'pfParticleTransformerAK4JetTags:probb',
     'pfParticleTransformerAK4JetTags:probbb',
     'pfParticleTransformerAK4JetTags:problepb',
     'pfParticleTransformerAK4JetTags:probc',
     'pfParticleTransformerAK4JetTags:probuds',
     'pfParticleTransformerAK4JetTags:probg',
 ] + _pfParticleNetFromMiniAODAK4PuppiCentralJetTagsProbs + pfParticleNetAK4JetTagsAll + _pfUnifiedParticleTransformerAK4JetTagsAll
else :
  bTagDiscriminators = [
      'pfDeepCSVJetTags:probudsg', #to be fixed with new names
      'pfDeepCSVJetTags:probb',
      'pfDeepCSVJetTags:probc',
      'pfDeepCSVJetTags:probbb',
      'pfDeepCSVJetTags:probcc',
      'pfDeepFlavourJetTags:probb',
      'pfDeepFlavourJetTags:probbb',
      'pfDeepFlavourJetTags:problepb',
      'pfDeepFlavourJetTags:probc',
      'pfDeepFlavourJetTags:probuds',
      'pfDeepFlavourJetTags:probg',
      'pfParticleTransformerAK4JetTags:probb',
      'pfParticleTransformerAK4JetTags:probbb',
      'pfParticleTransformerAK4JetTags:problepb',
      'pfParticleTransformerAK4JetTags:probc',
      'pfParticleTransformerAK4JetTags:probuds',
      'pfParticleTransformerAK4JetTags:probg',
 ] + _pfParticleNetFromMiniAODAK4PuppiCentralJetTagsProbs + pfParticleNetAK4JetTagsAll + _pfUnifiedParticleTransformerAK4JetTagsAll


#jetCorrectionsAK4 = ('AK4PF', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')

from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection


    
###########################################################################
#
# Setup puppi modules and set them to recalculate weights
#
###########################################################################
from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD
makePuppiesFromMiniAOD(process, False)
process.puppi.useExistingWeights = False
process.puppiNoLep.useExistingWeights = False

###########################################################################
#
# Make function wrapper around PatAlgos helper functions
#
###########################################################################
from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask, addToProcessAndTask
def addProcessAndTask(proc, label, module):
  task = getPatAlgosToolsTask(proc)
  addToProcessAndTask(label, module, proc, task)

###########################################################################
#
# Recluster AK4 Puppi jets
#
###########################################################################
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJetsPuppi
from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets

jetCollectionRecluster = ""




if not usePuppi:
   jetCorrectionsAK4 = ('AK4PFchs', [], 'None')
   addProcessAndTask(process, "packedPFCandidatesChg",cms.EDFilter("CandPtrSelector",
        src = cms.InputTag("packedPFCandidates"),
        cut = cms.string("charge != 0 && pvAssociationQuality>3")
       )
   )
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
        genJetCollection   = cms.InputTag("ak4GenChargedJetsRecluster"),
        genParticles       = cms.InputTag("prunedGenParticles"),
        jetCorrections     = jetCorrectionsAK4,
   )

   updateJetCollection(
        process,
        labelName = "AK4PFChgFinal",
        jetSource = cms.InputTag("selectedPatJetsAK4PFChg"),
        pfCandidates = cms.InputTag('packedPFCandidates'),
        pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        muSource = cms.InputTag('slimmedMuons'),
        elSource = cms.InputTag('slimmedElectrons'),
        jetCorrections = jetCorrectionsAK4,
        btagDiscriminators = bTagDiscriminators,
        btagInfos = bTagInfos,
        explicitJTA = False
   )
   jetCollectionRecluster='selectedUpdatedPatJetsAK4PFChgFinal'
   if hasattr(process,'updatedPatJetsTransientCorrectedAK4PFChgFinal'):
      process.updatedPatJetsTransientCorrectedAK4PFChgFinal.addTagInfos = cms.bool(True)
      process.updatedPatJetsTransientCorrectedAK4PFChgFinal.addBTagInfo = cms.bool(True)
   else:
      raise ValueError('I could not find updatedPatJetsTransientCorrectedCorrectedAK4PFChgFinal to embed the tagInfos, please check the cfg')



else:
  jetCorrectionsAK4 = ('AK4PFPuppi', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
  from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJetsPuppi
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
     algo               = "ak", #name of algo must be in this format
     rParam             = 0.4,
     pvSource           = cms.InputTag("offlineSlimmedPrimaryVertices"),
     pfCandidates       = cms.InputTag("packedPFCandidates"),
     svSource           = cms.InputTag("slimmedSecondaryVertices"),
     muSource           = cms.InputTag("slimmedMuons"),
     elSource           = cms.InputTag("slimmedElectrons"),
     genJetCollection   = cms.InputTag("ak4GenChargedJetsRecluster"), # This is setup below
     genParticles       = cms.InputTag("prunedGenParticles"),
     jetCorrections     = jetCorrectionsAK4,
   )
  
  process.patJetsAK4PuppiRecluster.getJetMCFlavour = True
  getattr(process, "patJetFlavourAssociationAK4PuppiRecluster").weights = cms.InputTag("puppi")
  
  updateJetCollection(
           process,
           labelName = "AK4PuppiFinal",
           jetSource = cms.InputTag("selectedPatJetsAK4PuppiRecluster"),  # 'ak4Jets'
           jetCorrections = jetCorrectionsAK4,
           pfCandidates = cms.InputTag('packedPFCandidates'),
           pvSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
           svSource = cms.InputTag('slimmedSecondaryVertices'),
           muSource = cms.InputTag('slimmedMuons'),
           elSource = cms.InputTag('slimmedElectrons'),
           btagInfos = bTagInfos,
           btagDiscriminators = bTagDiscriminators,
           explicitJTA = False
  )
  jetCollectionRecluster='selectedUpdatedPatJetsAK4PuppiFinal'
  #jetCollectionRecluster='slimmedJetsPuppi'
  if hasattr(process,'updatedPatJetsTransientCorrectedAK4PuppiFinal'):
    process.updatedPatJetsTransientCorrectedAK4PuppiFinal.addTagInfos = cms.bool(True) 
    process.updatedPatJetsTransientCorrectedAK4PuppiFinal.addBTagInfo = cms.bool(True)
  else:
    raise ValueError('I could not find updatedPatJetsTransientCorrectedPuppi to embed the tagInfos, please check the cfg')




# QGLikelihood
process.load("DeepNTuples.DeepNtuplizer.QGLikelihood_cfi")
process.es_prefer_jec = cms.ESPrefer("PoolDBESSource", "QGPoolDBESSource")
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag(jetCollectionRecluster)
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')


from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
process.ak4GenJetsWithNu = ak4GenJets.clone(src ='packedGenParticles')
 
 ## Filter out neutrinos from packed GenParticles
process.packedGenChargedParticles = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedGenParticles"), cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16 && charge!=0"))

## Define GenJets
process.ak4GenChargedJetsRecluster = ak4GenJets.clone(src = 'packedGenChargedParticles')

process.patGenJetMatchAllowDuplicates = cms.EDProducer("GenJetMatcher",  # cut on deltaR; pick best by deltaR           
    src         = cms.InputTag(jetCollectionRecluster),      # RECO jets (any View<Jet> is ok) 
    matched     = cms.InputTag("ak4GenChargedJetsRecluster"),        # GEN jets  (must be GenJetCollection)              
    mcPdgId     = cms.vint32(),                      # n/a   
    mcStatus    = cms.vint32(),                      # n/a   
    checkCharge = cms.bool(False),                   # n/a   
    maxDeltaR   = cms.double(0.4),                   # Minimum deltaR for the match   
    #maxDPtRel   = cms.double(3.0),                  # Minimum deltaPt/Pt for the match (not used in GenJetMatcher)                     
    resolveAmbiguities    = cms.bool(False),          # Forbid two RECO objects to match to the same GEN object 
    resolveByMatchQuality = cms.bool(False),         # False = just match input in order; True = pick lowest deltaR pair first          
)
 
 
process.patGenJetMatchWithNu = cms.EDProducer("GenJetMatcher",  # cut on deltaR; pick best by deltaR           
    src         = cms.InputTag(jetCollectionRecluster),      # RECO jets (any View<Jet> is ok) 
    matched     = cms.InputTag("ak4GenJetsWithNu"),        # GEN jets  (must be GenJetCollection)              
    mcPdgId     = cms.vint32(),                      # n/a   
    mcStatus    = cms.vint32(),                      # n/a   
    checkCharge = cms.bool(False),                   # n/a   
    maxDeltaR   = cms.double(0.4),                   # Minimum deltaR for the match   
    #maxDPtRel   = cms.double(3.0),                  # Minimum deltaPt/Pt for the match (not used in GenJetMatcher)                     
    resolveAmbiguities    = cms.bool(True),          # Forbid two RECO objects to match to the same GEN object 
    resolveByMatchQuality = cms.bool(False),         # False = just match input in order; True = pick lowest deltaR pair first          
)

process.patGenChargedJetMatchRecluster = cms.EDProducer("GenJetMatcher",  # cut on deltaR; pick best by deltaR           
    src         = cms.InputTag(jetCollectionRecluster),      # RECO jets (any View<Jet> is ok) 
    matched     = cms.InputTag("ak4GenChargedJetsRecluster"),        # GEN jets  (must be GenJetCollection)              
    mcPdgId     = cms.vint32(),                      # n/a   
    mcStatus    = cms.vint32(),                      # n/a   
    checkCharge = cms.bool(False),                   # n/a   
    maxDeltaR   = cms.double(0.4),                   # Minimum deltaR for the match   
    #maxDPtRel   = cms.double(3.0),                  # Minimum deltaPt/Pt for the match (not used in GenJetMatcher)                     
    resolveAmbiguities    = cms.bool(True),          # Forbid two RECO objects to match to the same GEN object 
    resolveByMatchQuality = cms.bool(False),         # False = just match input in order; True = pick lowest deltaR pair first          
)

process.genJetReclusterTask = cms.Task(process.packedGenChargedParticles,process.ak4GenJetsWithNu,process.ak4GenChargedJetsRecluster) 
process.genJetMatchTask = cms.Task(process.patGenJetMatchAllowDuplicates,process.patGenJetMatchWithNu,process.patGenChargedJetMatchRecluster)

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

# DeepNtuplizer
process.load("DeepNTuples.DeepNtuplizer.BareDeepNtuplizer_cfi")
#process.deepntuplizer.jets = cms.InputTag("slimmedJetsPuppi")
process.deepntuplizer.jets = cms.InputTag(jetCollectionRecluster)
process.deepntuplizer.genJets = cms.InputTag("ak4GenChargedJetsRecluster")
process.deepntuplizer.genJetMatchRecluster = cms.InputTag("patGenChargedJetMatchRecluster")
process.deepntuplizer.LooseSVs = cms.InputTag("looseIVFinclusiveCandidateSecondaryVertices")

process.deepntuplizer.applySelection = cms.bool(options.selectJets)

if ( int(releases[0]) > 8 ) or ( (int(releases[0])==8) and (int(releases[1]) >= 4) ):
   process.deepntuplizer.tagInfoName = cms.string('pfDeepCSV')
process.deepntuplizer.gluonReduction  = cms.double(options.gluonReduction)


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
    process.QGTagger + process.deepntuplizer,
    process.tsk, 
    process.patAlgosToolsTask, 
    process.genJetReclusterTask, 
    process.genJetMatchTask
)

