from CRABClient.UserUtilities import config, ClientException
from CRABAPI.RawCommand import crabCommand
import datetime
import os.path, subprocess


def list_of_files(path):
  files= subprocess.check_output(["ls", "/eos/cms/"+path]).splitlines()
  outfiles=''
  iline =0 
  for line in files:
    if b"root" in line:
       if iline<len(files):
          outfiles+="'"+path+(line.decode())+"',"
       else:
          outfiles+="'"+path+(line.decode())+"'"
    iline+=1
  return outfiles

def submit(config):
    crabCommand('submit', config = config)


templ_sub="from WMCore.Configuration import Configuration\n"
templ_sub += "config = Configuration()\n"
templ_sub += "config.section_('General')\n"
templ_sub +="config.General.transferOutputs = True\n"
###### name local
templ_sub +="config.General.workArea = 'crab_projects/PUPPI_Signal_cascade_m100_31'\n"
templ_sub +="config.section_('JobType')\n"
templ_sub +="config.JobType.pluginName = 'Analysis'\n"
####### Here put the code
templ_sub +="config.JobType.psetName = 'DeepNtuplizer_pfc2.py'\n"
######
templ_sub +="config.JobType.allowUndistributedCMSSW = True\n"
templ_sub +="config.JobType.maxMemoryMB = 3500\n"
templ_sub +='config.JobType.inputFiles = ["../python/QGL_cmssw8020_v2.db"]\n'
templ_sub +="config.section_('Data')\n"
templ_sub +="config.Data.splitting = 'FileBased'\n"
templ_sub +="config.Data.unitsPerJob = 20\n"
templ_sub +="config.Data.inputDBS = 'global'\n"
templ_sub +="config.Data.publication = False\n"
###### Name in eos
templ_sub +="config.Data.outputDatasetTag = 'PUPPI_Signal_cascade_m100_31'\n"
templ_sub +="config.section_('Site')\n"
templ_sub +="config.Site.storageSite = 'T2_CH_CERN'\n"
##### output folder
templ_sub +="config.Data.outLFNDirBase = '/store/group/cmst3/group/softJets/friti/deepntuplizer/ntuples_v2/'\n"
##### input
## chain dm20
templ_sub +="config.Data.userInputFiles = ["+list_of_files('/store/cmst3/group/softJets/common/signal_samples_140X/chain_m70_dm20_cfgRun24_140X_Run2024_test_03062025/Mini/')+"]\n"
## chain dm8
templ_sub +="config.Data.userInputFiles = ["+list_of_files('/store/cmst3/group/softJets/common/signal_samples_140X/chain_m70_dm8_cfgRun24_140X_Run2024_test_03062025/Mini/')+"]\n"
## cascade m100
templ_sub +="config.Data.userInputFiles = ["+list_of_files('/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m100_31_cfgRun24_140X_Run2024_test_03062025/Mini/')+"]\n"
## cascade m220
templ_sub +="config.Data.userInputFiles = ["+list_of_files('/store/cmst3/group/softJets/common/signal_samples_140X/cascade_m220_67_20_cfgRun24_140X_Run2024_test_03062025/Mini/')+"]\n"

with open("to_sub.py","w") as txt:
   txt.write(templ_sub)
txt.close()
os.system("crab submit -c to_sub.py")

