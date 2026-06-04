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
    crab_config.JobType.maxMemoryMB = 3000
    crab_config.JobType.inputFiles = ["../python/QGL_cmssw8020_v2.db"]

    # Data
    crab_config.Data.splitting = 'FileBased'
    crab_config.Data.unitsPerJob = 5
    crab_config.Data.inputDBS = 'global'
    crab_config.Data.publication = False
    crab_config.Data.outputDatasetTag = output_tag
    crab_config.Data.outLFNDirBase = '/store/group/cmst3/group/softJets/friti/deepntuplizer/2023/ntuples_3June2026_withgenbranch/'


    # DAS dataset input
    crab_config.Data.inputDataset = dataset_path

    # Site
    crab_config.Site.storageSite = 'T2_CH_CERN'

    # Print info before submission

    print("Submitting CRAB job for dataset: {}".format(dataset_path))
    print("WorkArea (CRAB project folder): {}".format(crab_config.General.workArea))
    print("RequestName (CRAB task name): {}".format(crab_config.General.requestName))

    output_folder = os.path.join(crab_config.Data.outLFNDirBase, output_tag)
    print("Expected output folder on EOS: {}".format(output_folder))

    crabCommand('submit', config=crab_config)

    return crab_config.General.workArea  # return the workarea for status check

# Get today's date string
today_str = datetime.datetime.now().strftime("%Y-%m-%d")
base_workarea = os.path.join("crab_projects", today_str)

datasets = [
    # Replace the 'path' value with the DAS dataset name for each dataset below
    {
        'path': '/DisorderChain_m-70_dm-20_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15-v2/MINIAODSIM',  
        'folder': 'chain_m70_dm20',
        'tag': 'PUPPI_Signal_chain_m70_dm20'
    },
    {
        'path': '/DisorderChain_m-70_dm-20_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15_ext1-v4/MINIAODSIM',  
        'folder': 'chain_m70_dm20_ext',
        'tag': 'PUPPI_Signal_chain_m70_dm20_ext'
    },
    {
        'path': '/DisorderChain_m-70_dm-8_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15-v2/MINIAODSIM',  
        'folder': 'chain_m70_dm8',
        'tag': 'PUPPI_Signal_chain_m70_dm8'
    },
    {
        'path': '/DisorderChain_m-70_dm-8_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15_ext1-v4/MINIAODSIM',  
        'folder': 'chain_m70_dm8_ext',
        'tag': 'PUPPI_Signal_chain_m70_dm8_ext'
    },
    {
        'path': '/DisorderCascade_mPhi3-100-mPhi2-31-mPhi1-8_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15-v2/MINIAODSIM', 
        'folder': 'cascade_m100_31',
        'tag': 'PUPPI_Signal_cascade_m100_31'
    },
    {
        'path': '/DisorderCascade_mPhi3-100-mPhi2-31-mPhi1-8_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15_ext1-v4/MINIAODSIM',  
        'folder': 'cascade_m100_31_ext',
        'tag': 'PUPPI_Signal_cascade_m100_31_ext'
    },
    {
        'path': '/DisorderCascade_mPhi3-220-mPhi2-67-mPhi1-20_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15-v2/MINIAODSIM',  
        'folder': 'cascade_m220_67_20',
        'tag': 'PUPPI_Signal_cascade_m220_67_20'
    },
    {
        'path': '/DisorderCascade_mPhi3-220-mPhi2-67-mPhi1-20_TuneCP5_13p6TeV_madgraph-pythia8/Run3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v15_ext1-v4/MINIAODSIM',  
        'folder': 'cascade_m220_67_20_ext',
        'tag': 'PUPPI_Signal_cascade_m220_67_20_ext'
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
        crab_project_dir = "{}/crab_{}".format(wa, ds['tag'])
        f.write("echo '=== Status for {} ==='\n".format(crab_project_dir))
        f.write("crab status -d {}\n\n".format(crab_project_dir))

os.chmod(script_filename, 0o755)  # make it executable

print("\nCreated status checking script: {}".format(script_filename))
print("Run it with: ./{}".format(script_filename))
