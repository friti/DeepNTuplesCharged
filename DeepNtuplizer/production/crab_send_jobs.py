import datetime
from CRABClient.UserUtilities import config
from CRABAPI.RawCommand import crabCommand
import subprocess
import os

def list_of_files(path):
    if not path.endswith('/'):
        path += '/'
    files = subprocess.check_output(["ls", "/eos/cms/"+path]).splitlines()
    outfiles = []
    for line in files:
        fname = line.decode()
        if "root" in fname:
            outfiles.append(path + fname)
    return outfiles

def submit_crab_job(dataset_path, base_workarea, folder_name, output_tag):
    crab_config = config()

    # General
    crab_config.General.transferOutputs = True
    crab_config.General.workArea = os.path.join(base_workarea, folder_name)
    crab_config.General.requestName = output_tag

    # JobType
    crab_config.JobType.pluginName = 'Analysis'
    crab_config.JobType.psetName = 'DeepNtuplizer_pfc2.py'
    crab_config.JobType.allowUndistributedCMSSW = True
    crab_config.JobType.maxMemoryMB = 3500
    crab_config.JobType.maxJobRuntimeMin = 3000
    crab_config.JobType.inputFiles = ["../python/QGL_cmssw8020_v2.db"]

    # Data
    crab_config.Data.splitting = 'FileBased'
    crab_config.Data.unitsPerJob = 2
    crab_config.Data.inputDBS = 'global'
    crab_config.Data.publication = False
    crab_config.Data.outputDatasetTag = output_tag
    crab_config.Data.outLFNDirBase = '/store/group/cmst3/group/bpark/friti/deepntuplizer/ntuples_Oct1/'

    '''
    # Input files
    input_files = list_of_files(dataset_path)
    if len(input_files) == 0:
        print(f"WARNING: No root files found for dataset {dataset_path}")
    crab_config.Data.userInputFiles = input_files
    '''

    crab_config.Data.inputDataset = dataset_path

    # Site
    crab_config.Site.storageSite = 'T2_CH_CERN'

    # Print info before submission
    print(f"Submitting CRAB job for dataset: {dataset_path}")
    print(f"WorkArea (CRAB project folder): {crab_config.General.workArea}")
    print(f"RequestName (CRAB task name): {crab_config.General.requestName}")

    output_folder = os.path.join(crab_config.Data.outLFNDirBase, output_tag)
    print(f"Expected output folder on EOS: {output_folder}")

    crabCommand('submit', config=crab_config)

    return crab_config.General.workArea  # return the workarea for status check

# Get today's date string
today_str = datetime.datetime.now().strftime("%Y-%m-%d")
base_workarea = os.path.join("crab_projects", today_str)

datasets = [
    {
        'path': '/ttbarToBsToTauTau_BsFilter_TauTauFilter_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2/MINIAODSIM',
        'folder': 'bstautau_sm_old',
        'tag': 'PUPPI_Signal_sm_old'
    },
    {
        'path': '/TTToBsToTauTau_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_sm_new',
        'tag': 'PUPPI_Signal_sm_new'
    },
    {
        'path': '/TTToBsToTauTau_MBs-10_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2/MINIAODSIM',
        'folder': 'bstautau_mbs10',
        'tag': 'PUPPI_Signal_mbs10'
    },
    {
        'path': '/TTToBsToTauTau_MBs-3p6_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs3p6',
        'tag': 'PUPPI_Signal_mbs3p6'
    },
    {
        'path': '/TTToBsToTauTau_MBs-4_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs4',
        'tag': 'PUPPI_Signal_mbs4'
    },
    {
        'path': '/TTToBsToTauTau_MBs-4p5_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs4p5',
        'tag': 'PUPPI_Signal_mbs4p5'
    },
    {
        'path': '/TTToBsToTauTau_MBs-5p5_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs5p5',
        'tag': 'PUPPI_Signal_mbs5p5'
    },
    {
        'path': '/TTToBsToTauTau_MBs-6_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs6',
        'tag': 'PUPPI_Signal_mbs6'
    },
    {
        'path': '/TTToBsToTauTau_MBs-7_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs7',
        'tag': 'PUPPI_Signal_mbs7'
    },
    {
        'path': '/TTToBsToTauTau_MBs-7p5_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs7p5',
        'tag': 'PUPPI_Signal_mbs7p5'
    },
    {
        'path': '/TTToBsToTauTau_MBs-8_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs8',
        'tag': 'PUPPI_Signal_mbs8'
    },
    {
        'path': '/TTToBsToTauTau_MBs-9_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs9',
        'tag': 'PUPPI_Signal_mbs9'
    },
    {
        'path': '/TTToBsToTauTau_MBs-9p5_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v3/MINIAODSIM',
        'folder': 'bstautau_mbs9p5',
        'tag': 'PUPPI_Signal_mbs9p5'
    },
    {
        'path': '/TTToBsToTauTau_MBs-10_TuneCP5_13TeV-pythia8-evtgen/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2/MINIAODSIM',
        'folder': 'bstautau_mbs10',
        'tag': 'PUPPI_Signal_mbs10'
    },
    {
        'path': '/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2/MINIAODSIM',
        'folder': 'ttsemilep',
        'tag': 'PUPPI_bkg_ttsemilep'
    },
]

workareas = []
for ds in datasets:
    workarea = submit_crab_job(ds['path'], base_workarea, ds['folder'], ds['tag'])
    workareas.append(workarea)

# Create a status-checking bash script
script_filename = "check_crab_jobs.sh"
with open(script_filename, "w") as f:
    f.write("#!/bin/bash\n\n")
    f.write("# Script to check status of all submitted CRAB jobs\n\n")
    for wa, ds in zip(workareas, datasets):
        crab_project_dir = f"{wa}/crab_{ds['tag']}"
        f.write(f"echo '=== Status for {crab_project_dir} ==='\n")
        f.write(f"crab status -d {crab_project_dir}\n\n")

os.chmod(script_filename, 0o755)  # make it executable

print(f"\nCreated status checking script: {script_filename}")
print(f"Run it with: ./{script_filename}")
