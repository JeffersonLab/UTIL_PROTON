// 31/08/20 - Stephen Kay, University of Regina

// root .c macro plotting script, reads in desired trees from analysed root file and plots some stuff
// This version uses the tree WITHOUT the RF cut to plot the final histograms
// Saves  pdf file with plots and a .root file
#define PlotProtonPhysics_NoRFCut_cxx

// Include relevant stuff
#include <TStyle.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TGaxis.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <TSystem.h>
#include <TTree.h>
#include <TArc.h>

// Input should be the input root file name (including suffix) and an output file name string (without any suffix)
void PlotProtonPhysics_NoRFCut(string InFilename = "", string OutFilename = "")
{
  TString Hostname = gSystem->HostName();
  TString User = (gSystem->GetUserInfo())->fUser;
  TString Replaypath;
  TString Outpath;
  TString rootFile;
  Double_t nWindows = 6;
  gStyle->SetPalette(55);

  // Set paths depending on system you're running on
  if(Hostname.Contains("farm")){
    Replaypath = "/group/c-pionlt/USERS/"+User+"/hallc_replay_lt";
    Outpath = Replaypath+"/UTIL_PROTON/OUTPUT/Analysis/ProtonLT";
  }
  else if(Hostname.Contains("qcd")){
    Replaypath = "/group/c-pionlt/USERS/"+User+"/hallc_replay_lt";
    Outpath = Replaypath+"/UTIL_PROTON/OUTPUT/Analysis/ProtonLT";
  }
  else if (Hostname.Contains("phys.uregina.ca")){
    Replaypath = "/home/"+User+"/work/JLab/hallc_replay_lt";
    Outpath = Replaypath+"/UTIL_PROTON/OUTPUT/Analysis/ProtonLT";
  }
  // Add more as needed for your own envrionment
  else{
    Replaypath="/ExtDsk/sjdkay/JLab/Proton_Analysis/OUTPUT/Pass2_3";
    Outpath = Replaypath;
  }

  if(InFilename == "") {
    cout << "Enter a Filename to analyse: ";
    cin >> InFilename;
  }  
  if(OutFilename == "") {
    cout << "Enter a Filename to output to: ";
    cin >> OutFilename;
  }
  TString TInFilename = InFilename;
  rootFile = Outpath+"/"+TInFilename;
  // Complain and exit if your file doesn't exist
  if (gSystem->AccessPathName(rootFile) == kTRUE){
    cerr << "!!!!! ERROR !!!!! " << endl << rootFile <<  " not found" << endl <<  "!!!!! ERRROR !!!!!" << endl;
    exit;
  }
  TFile *InFile = new TFile(rootFile, "READ");
  // This assumes a 4 digit run number! Should work for now, saves it as an additional input
  // This ALSO assumes the Filename is XXXX_..... too, which may not be true, edit as needed
  TString TOutFilename = OutFilename;
  // Establish the names of our output files quickly
  TString foutname = Outpath + "/" + TOutFilename + ".root";
  TString foutpdf = Outpath + "/" + TOutFilename + ".pdf";
  
  TTree* Uncut = (TTree*)InFile->Get("Uncut_Proton_Events"); Long64_t nEntries_Uncut = (Long64_t)Uncut->GetEntries();
  TTree* Cut_All = (TTree*)InFile->Get("Cut_Proton_Events_All"); Long64_t nEntries_All = (Long64_t)Cut_All->GetEntries();
  TTree* Cut_All_NoRF = (TTree*)InFile->Get("Cut_Proton_Events_All_NoRF"); Long64_t nEntries_All_NoRF = (Long64_t)Cut_All_NoRF->GetEntries();
  TTree* Cut_Pr = (TTree*)InFile->Get("Cut_Proton_Events_Prompt_NoRF"); Long64_t nEntries_Pr = (Long64_t)Cut_Pr->GetEntries();
  TTree* Cut_Rn = (TTree*)InFile->Get("Cut_Proton_Events_Random_NoRF"); Long64_t nEntries_Rn = (Long64_t)Cut_Rn->GetEntries();
  // Set branch address -> Need this to ensure event info is entangled correctly for 2D plots
  Double_t CT_all; Cut_All_NoRF->SetBranchAddress("CTime_epCoinTime_ROC1", &CT_all);
  Double_t CT_pr; Cut_Pr->SetBranchAddress("CTime_epCoinTime_ROC1", &CT_pr);
  Double_t CT_rn; Cut_Rn->SetBranchAddress("CTime_epCoinTime_ROC1", &CT_rn);
  Double_t Beta_all; Cut_All_NoRF->SetBranchAddress("P_gtr_beta", &Beta_all);
  Double_t Beta_pr; Cut_Pr->SetBranchAddress("P_gtr_beta", &Beta_pr);
  Double_t Beta_rn; Cut_Rn->SetBranchAddress("P_gtr_beta", &Beta_rn);
  Double_t MMp_all; Cut_All_NoRF->SetBranchAddress("MMp", &MMp_all);
  Double_t MMp_pr; Cut_Pr->SetBranchAddress("MMp", &MMp_pr);
  Double_t MMp_rn; Cut_Rn->SetBranchAddress("MMp", &MMp_rn);
  Double_t W_pr; Cut_Pr->SetBranchAddress("W", &W_pr);
  Double_t Q2_pr; Cut_Pr->SetBranchAddress("Q2", &Q2_pr);
  Double_t t_pr; Cut_Pr->SetBranchAddress("MandelT", &t_pr);
  Double_t phi_q_pr; Cut_Pr->SetBranchAddress("ph_q", &phi_q_pr);
  // Quantities for PID cuts/comparisons
  Double_t HMSCal_uncut; Uncut->SetBranchAddress("H_cal_etotnorm", &HMSCal_uncut);
  Double_t HMSCal_cut; Cut_All_NoRF->SetBranchAddress("H_cal_etotnorm", &HMSCal_cut);
  Double_t HMSCalTrack_uncut; Uncut->SetBranchAddress("H_cal_etottracknorm", &HMSCalTrack_uncut);
  Double_t HMSCalTrack_cut; Cut_All_NoRF->SetBranchAddress("H_cal_etottracknorm", &HMSCalTrack_cut);
  Double_t HMSCher_uncut; Uncut->SetBranchAddress("H_cer_npeSum", &HMSCher_uncut);
  Double_t HMSCher_cut; Cut_All_NoRF->SetBranchAddress("H_cer_npeSum", &HMSCher_cut);
  Double_t AeroNPE_uncut; Uncut->SetBranchAddress("P_aero_npeSum", &AeroNPE_uncut);
  Double_t HGCNPE_uncut; Uncut->SetBranchAddress("P_hgcer_npeSum", &HGCNPE_uncut);
  Double_t AeroNPE_all; Cut_All_NoRF->SetBranchAddress("P_aero_npeSum", &AeroNPE_all);
  Double_t HGCNPE_all; Cut_All_NoRF->SetBranchAddress("P_hgcer_npeSum", &HGCNPE_all);
  
  // Define Histograms
  TH1D *h1_MMp_All = new TH1D("h1_MMp_All", "MM_{p} - All events after cuts; Mass (GeV/c^{2})", 200, 0.0, 2.0);
  TH1D *h1_MMp_Prompt = new TH1D("h1_MMp_Prompt", "MM_{p} - Prompt events after cuts; Mass (GeV/c^{2})", 200, 0.0, 2.0);
  TH1D *h1_MMp_Random = new TH1D("h1_MMp_Random", "MM_{p} - Random events after cuts; Mass (GeV/c^{2})", 200, 0.0, 2.0);
  TH1D *h1_MMp_Random_Scaled = new TH1D("h1_MMp_Random_Scaled", "MM_{p} - Random events after cuts; Mass (GeV/c^{2})", 200, 0.0, 2.0);
  TH1D *h1_MMp_BGSub = new TH1D("h1_MMp_BGSub", "MM_{p} - BGSub events after cuts; Mass (GeV/c^{2})", 200, 0.0, 2.0);
  TH1D *h1_CT_All = new TH1D("h1_CT_All", "Proton CT - All events after cuts; Time (ns)", 240, 10, 70); 
  TH1D *h1_CT_Prompt = new TH1D("h1_CT_Prompt", "Proton CT - Prompt events after cuts; Time (ns)", 240, 10, 70);
  TH1D *h1_CT_Random = new TH1D("h1_CT_Random", "Proton CT - Random events after cuts; Time (ns)", 240, 10, 70);
  TH1D *h1_Epsilon = new TH1D("h1_Epsilon", "#epsilon - Prompt events after cuts; #epsilon", 200, 0, 1);

  TH1D *h1_RFCutDist = new TH1D("h1_RFCutDist", "RFCutDist - No RF or PID Cut applied", 200, 0, 4);
  TH1D *h1_RFCutDist_woCut = new TH1D("h1_RFCutDist_woCut", "Proton RFCutDist - No RF Cut applied", 200, 0, 4);
  TH1D *h1_RFCutDist_wCut = new TH1D("h1_RFCutDist_wCut", "Proton RFCutDist - RF Cut applied", 200, 0, 4);

  TH1D *h1_Aero_Uncut = new TH1D("h1_Aero_Uncut", "Aerogel NPESum - all events before cuts", 50, 0, 50);
  TH1D *h1_Aero_Cut = new TH1D("h1_Aero_Cut", "Aerogel NPESum - all events after cuts", 50, 0, 50);
  TH1D *h1_HGC_Uncut = new TH1D("h1_HGC_Uncut", "HGC NPESum - all events before cuts", 50, 0, 50);
  TH1D *h1_HGC_Cut = new TH1D("h1_HGC_Cut", "HGC NPESum - all events after cuts", 50, 0, 50);
  TH1D *h1_HCal_Uncut = new TH1D("h1_HCal_Uncut", "HMS Normalised Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_HCal_Cut = new TH1D("h1_HCal_Cut", "HMS Normalised Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_HCalTrackUncut = new TH1D("h1_HCalTrackUncut", "HMS Normalised Track Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_HCalTrackCut = new TH1D("h1_HCalTrackCut", "HMS Normalised Track Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_PCal_Uncut = new TH1D("h1_PCal_Uncut", "SHMS Normalised Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_PCal_Cut = new TH1D("h1_PCal_Cut", "SHMS Normalised Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_PCalTrack_Uncut = new TH1D("h1_PCalTrack_Uncut", "SHMS Normalised Track Calorimeter Energy - all events before cuts", 150, 0, 1.5);
  TH1D *h1_PCalTrack_Cut = new TH1D("h1_PCalTrack_Cut", "SHMS Normalised Trcak Calorimeter Energy - all events before cuts", 150, 0, 1.5);

  TH1D *h1_HDelta_Uncut = new TH1D("h1_HDelta_Uncut", "HMS #delta - all events before cuts", 400, -20, 20);
  TH1D *h1_HDelta_Cut = new TH1D("h1_HDelta_Cut", "HMS #delta - all events after cuts", 400, -20, 20);
  TH1D *h1_PDelta_Uncut = new TH1D("h1_PDelta_Uncut", "SHMS #delta - all events before cuts", 400, -20, 20);
  TH1D *h1_PDelta_Cut = new TH1D("h1_PDelta_Cut", "SHMS #delta - all events after cuts", 400, -20, 20);
  TH1D *h1_Hxp_Uncut = new TH1D("h1_Hxp_Uncut", "HMS x' - all events before cuts", 200, -0.1, 0.1);
  TH1D *h1_Hxp_Cut = new TH1D("h1_Hxp_Cut", "HMS x' - all events after cuts", 200, -0.1, 0.1);
  TH1D *h1_Pxp_Uncut = new TH1D("h1_Pxp_Uncut", "SHMS x' - all events before cuts", 200, -0.1, 0.1);
  TH1D *h1_Pxp_Cut = new TH1D("h1_Pxp_Cut", "SHMS x' - all events after cuts", 200, -0.1, 0.1);
  TH1D *h1_Hyp_Uncut = new TH1D("h1_Hyp_Uncut", "HMS y' - all events before cuts", 200, -0.1, 0.1);
  TH1D *h1_Hyp_Cut = new TH1D("h1_Hyp_Cut", "HMS y' - all events after cuts", 200, -0.1, 0.1);
  TH1D *h1_Pyp_Uncut = new TH1D("h1_Pyp_Uncut", "SHMS y' - all events before cuts", 200, -0.1, 0.1);
  TH1D *h1_Pyp_Cut = new TH1D("h1_Pyp_Cut", "SHMS y' - all events after cuts", 200, -0.1, 0.1);

  TH2D *h2_HMS_CalCher_Uncut = new TH2D("h2_HMS_CalCher_Uncut", "HMS Calorimeter E_{TotNorm} vs HMS Cherenkov NPE - all events before cuts; HMS Cal E_{TotNorm}; HMS Cherenkov NPE", 150, 0, 1.5, 100, 0, 50);
  TH2D *h2_HMS_CalCher_Cut = new TH2D("h2_HMS_CalCher_Cut", "HMS Calorimeter E_{TotNorm} vs HMS Cherenkov NPE - all events after cuts; HMS Cal E_{TotNorm}; HMS Cherenkov NPE", 150, 0, 1.5, 100, 0, 50);

  TH2D *h2_HMS_CalTrackCher_Uncut = new TH2D("h2_HMS_CalTrackCher_Uncut", "HMS Calorimeter E_{TotTrackNorm} vs HMS Cherenkov NPE - all events before cuts; HMS Cal E_{TotTrackNorm}; HMS Cherenkov NPE", 150, 0, 1.5, 100, 0, 50);
  TH2D *h2_HMS_CalTrackCher_Cut = new TH2D("h2_HMS_CalTraclCher_Cut", "HMS Calorimeter E_{TotTrackNorm} vs HMS Cherenkov NPE - all events after cuts; HMS Cal E_{TotTrackNorm}; HMS Cherenkov NPE", 150, 0, 1.5, 100, 0, 50);

  TH2D *h2_AeroHGC_Uncut = new TH2D("h2_AeroHGC_Uncut", "Aerogel vs HGC NPESum - all events before cuts; Aerogel NPE; HGC NPE", 250, 0, 50, 250, 0, 250); 
  TH2D *h2_AeroHGC_Cut = new TH2D("h2_AeroHGC_Cut", "Aerogel vs HGC NPESum - all events after cuts; Aerogel NPE; HGC NPE", 250, 0, 250, 250, 0, 250); 
  
  TH2D *h2_Q2vsW = new TH2D("h2_Q2vsW","Q2 vs W;Q2;W", 200, 2.5, 4.5, 200, 2.7, 3.7);
  TH2D *h2_phiqvst = new TH2D("h2_phiqvst",";#phi;t",12,-3.14,3.14,40,0.0,2.0); 

  TH2D *h2_CT_Beta_All = new TH2D("h2_CT_Beta_All","Proton CT vs #beta - All events after cuts;Time (ns);#beta",240,10,70,80,0.6,1.4);
  TH2D *h2_CT_Beta_Prompt = new TH2D("h2_CT_Beta_Prompt","Proton CT vs #beta - Prompt events after cuts;Time (ns);#beta",240,10,70,80,0.6,1.4);
  TH2D *h2_CT_Beta_Random = new TH2D("h2_CT_Beta_Random","Proton CT vs #beta - Random events after cuts;Time (ns);#beta",240,10,70,80,0.6,1.4);

  TH2D *h2_CT_MMp_All = new TH2D("h2_CT_MMp_All","Proton CT vs MM_{p} - All events after cuts;Time (ns);Mass (GeV/c^{2})",240,10,70,150,0.5,2.0);
  TH2D *h2_CT_MMp_Prompt = new TH2D("h2_CT_MMp_Prompt","Proton CT vs MM_{p} - Prompt events after cuts;Time (ns);Mass (GeV/c^{2})",240,10,70,150,0.5,2.0);
  TH2D *h2_CT_MMp_Random = new TH2D("h2_CT_MMp_Random","Proton CT vs MM_{p} - Random events after cuts;Time (ns);Mass (GeV/c^{2})",240,10,70,150,0.5,2.0);

  // For 1D histos, can easily create directly from the corresponding branch
  Cut_All_NoRF->Draw("MMp >> h1_MMp_All", "", "goff");
  Cut_Pr->Draw("MMp >> h1_MMp_Prompt", "", "goff");
  Cut_Rn->Draw("MMp >> h1_MMp_Random", "", "goff");
  Cut_Rn->Draw("MMp  >> h1_MMp_Random_Scaled", "", "goff");
  Cut_All_NoRF->Draw("CTime_epCoinTime_ROC1 >> h1_CT_All", "", "goff");
  Cut_Pr->Draw("CTime_epCoinTime_ROC1 >> h1_CT_Prompt", "", "goff");
  Cut_Rn->Draw("CTime_epCoinTime_ROC1 >> h1_CT_Random", "", "goff");
  Cut_Pr->Draw("epsilon >> h1_Epsilon", "", "goff");

  Uncut->Draw("P_aero_npeSum >> h1_Aero_Uncut", "", "goff");
  Uncut->Draw("P_hgcer_npeSum >> h1_HGC_Uncut", "", "goff");
  Uncut->Draw("H_cal_etotnorm >> h1_HCal_Uncut", "", "goff");
  Uncut->Draw("H_cal_etottracknorm >> h1_HCalTrack_Uncut", "", "goff");
  Uncut->Draw("P_cal_etotnorm >> h1_PCal_Uncut", "", "goff");
  Uncut->Draw("P_cal_etottracknorm >> h1_PCalTrack_Uncut", "", "goff");
  Uncut->Draw("H_gtr_dp >> h1_HDelta_Uncut", "", "goff");
  Uncut->Draw("H_gtr_xp >> h1_Hxp_Uncut", "", "goff");
  Uncut->Draw("H_gtr_yp >> h1_Hyp_Uncut", "", "goff");
  Uncut->Draw("P_gtr_dp >> h1_PDelta_Uncut", "", "goff");
  Uncut->Draw("P_gtr_xp >> h1_Pxp_Uncut", "", "goff");
  Uncut->Draw("P_gtr_yp >> h1_Pyp_Uncut", "", "goff");
  Cut_All_NoRF->Draw("P_aero_npeSum >> h1_Aero_Cut", "", "goff");
  Cut_All_NoRF->Draw("P_hgcer_npeSum >> h1_HGC_Cut", "", "goff");
  Cut_All_NoRF->Draw("H_cal_etotnorm >> h1_HCal_Cut", "", "goff");
  Cut_All_NoRF->Draw("H_cal_etottracknorm >> h1_HCalTrack_Cut", "", "goff");
  Cut_All_NoRF->Draw("P_cal_etotnorm >> h1_PCal_Cut", "", "goff");
  Cut_All_NoRF->Draw("P_cal_etottracknorm >> h1_PCalTrack_Cut", "", "goff");
  Cut_All_NoRF->Draw("H_gtr_dp >> h1_HDelta_Cut", "", "goff");
  Cut_All_NoRF->Draw("H_gtr_xp >> h1_Hxp_Cut", "", "goff");
  Cut_All_NoRF->Draw("H_gtr_yp >> h1_Hyp_Cut", "", "goff");
  Cut_All_NoRF->Draw("P_gtr_dp >> h1_PDelta_Cut", "", "goff");
  Cut_All_NoRF->Draw("P_gtr_xp >> h1_Pxp_Cut", "", "goff");
  Cut_All_NoRF->Draw("P_gtr_yp >> h1_Pyp_Cut", "", "goff");
  
  Uncut->Draw("RF_CutDist >> h1_RFCutDist", "", "goff");
  Cut_All_NoRF->Draw("RF_CutDist >> h1_RFCutDist_woCut", "", "goff");
  Cut_All->Draw("RF_CutDist >> h1_RFCutDist_wCut", "", "goff");

  h1_MMp_Random_Scaled->Scale(1.0/nWindows);
  h1_MMp_BGSub->Add(h1_MMp_Prompt, h1_MMp_Random_Scaled, 1, -1);

  // Loop over all events in tree and fill 2D histos event by event, ensures the events correctly correlate
  for(Long64_t i = 0; i < nEntries_Uncut; i++){
    Uncut->GetEntry(i);
    h2_HMS_CalCher_Uncut->Fill(HMSCal_uncut,HMSCher_uncut);
    h2_HMS_CalTrackCher_Uncut->Fill(HMSCalTrack_uncut,HMSCher_uncut);
    h2_AeroHGC_Uncut->Fill(AeroNPE_uncut, HGCNPE_uncut);
  } 
  for(Long64_t i = 0; i < nEntries_All_NoRF; i++){
    Cut_All_NoRF->GetEntry(i);
    h2_CT_Beta_All->Fill(CT_all, Beta_all);
    h2_CT_MMp_All->Fill(CT_all, MMp_all);
    h2_HMS_CalCher_Cut->Fill(HMSCal_cut,HMSCher_cut);
    h2_HMS_CalTrackCher_Cut->Fill(HMSCalTrack_cut,HMSCher_cut);
    h2_AeroHGC_Cut->Fill(AeroNPE_all, HGCNPE_all);
  } 
  for(Long64_t i = 0; i < nEntries_Pr; i++){
    Cut_Pr->GetEntry(i);
    h2_CT_Beta_Prompt->Fill(CT_pr, Beta_pr);
    h2_CT_MMp_Prompt->Fill(CT_pr, MMp_pr);
    h2_Q2vsW->Fill(Q2_pr, W_pr);
    h2_phiqvst->Fill(phi_q_pr, -t_pr);
  }
  for(Long64_t i = 0; i < nEntries_Rn; i++){
    Cut_Rn->GetEntry(i);
    h2_CT_Beta_Random->Fill(CT_rn, Beta_rn);
    h2_CT_MMp_Random->Fill(CT_rn, MMp_rn);
  } 
  
  TCanvas *c_MM = new TCanvas("c_MM", "Proton missing mass distributions", 100, 0, 1000, 900);
  c_MM->Divide(2,2);
  c_MM->cd(1);
  h1_MMp_All->Draw();
  c_MM->cd(2);
  h1_MMp_Prompt->Draw();
  c_MM->cd(3);
  h1_MMp_Random->Draw();
  c_MM->cd(4);
  h1_MMp_BGSub->Draw("HIST");
  c_MM->Print(foutpdf + '(');

  TCanvas *c_Track = new TCanvas("c_Track", "Tracking cut distributions", 100, 0, 1000, 900);  
  c_Track->Divide(3,2);
  c_Track->cd(1);
  h1_HDelta_Uncut->SetLineColor(2); h1_HDelta_Cut->SetLineColor(4);
  h1_HDelta_Uncut->Draw("HIST"); h1_HDelta_Cut->Draw("HISTSAME");
  c_Track->cd(2); gPad->SetLogy();
  h1_PDelta_Uncut->SetLineColor(2); h1_PDelta_Cut->SetLineColor(4);
  h1_PDelta_Uncut->Draw("HIST"); h1_PDelta_Cut->Draw("HISTSAME");
  c_Track->cd(3); gPad->SetLogy();
  h1_Hxp_Uncut->SetLineColor(2); h1_Hxp_Cut->SetLineColor(4);
  h1_Hxp_Uncut->Draw("HIST"); h1_Hxp_Cut->Draw("HISTSAME");
  c_Track->cd(4); gPad->SetLogy();
  h1_Pxp_Uncut->SetLineColor(2); h1_Pxp_Cut->SetLineColor(4);
  h1_Pxp_Uncut->Draw("HIST"); h1_Pxp_Cut->Draw("HISTSAME");
  c_Track->cd(5); gPad->SetLogy();
  h1_Hyp_Uncut->SetLineColor(2); h1_Hyp_Cut->SetLineColor(4);
  h1_Hyp_Uncut->Draw("HIST"); h1_Hyp_Cut->Draw("HISTSAME");
  c_Track->cd(6); gPad->SetLogy();
  h1_Pyp_Uncut->SetLineColor(2); h1_Pyp_Cut->SetLineColor(4);
  h1_Pyp_Uncut->Draw("HIST"); h1_Pyp_Cut->Draw("HISTSAME");
  c_Track->Print(foutpdf);

  TCanvas *c_PID = new TCanvas("c_PID", "PID cut distributions", 100, 0, 1000, 900);  
  c_PID->Divide(5,2);
  c_PID->cd(1); gPad->SetLogy();
  h1_Aero_Uncut->SetLineColor(2); h1_Aero_Cut->SetLineColor(4);
  h1_Aero_Uncut->Draw("HIST"); h1_Aero_Cut->Draw("HISTSAME");
  c_PID->cd(2); gPad->SetLogy();
  h1_HGC_Uncut->SetLineColor(2); h1_HGC_Cut->SetLineColor(4);
  h1_HGC_Uncut->Draw("HIST"); h1_HGC_Cut->Draw("HISTSAME");
  c_PID->cd(3); gPad->SetLogy();
  h1_HCal_Uncut->SetLineColor(2); h1_HCal_Cut->SetLineColor(4);
  h1_HCal_Uncut->Draw("HIST"); h1_HCal_Cut->Draw("HISTSAME");
  c_PID->cd(4); gPad->SetLogy();
  h1_PCal_Uncut->SetLineColor(2); h1_PCal_Cut->SetLineColor(4);
  h1_PCal_Uncut->Draw("HIST"); h1_PCal_Cut->Draw("HISTSAME");
  c_PID->cd(5); gPad->SetLogz();
  c_PID->SetLogy(0); c_PID->SetLogz(1);
  h2_AeroHGC_Uncut->Draw("COLZ");
  c_PID->cd(6); gPad->SetLogz();
  h2_AeroHGC_Cut->Draw("COLZ"); 
  c_PID->cd(7); gPad->SetLogz();
  h2_HMS_CalCher_Uncut->Draw("COLZ");
  c_PID->cd(8); gPad->SetLogz();
  h2_HMS_CalCher_Cut->Draw("COLZ");
  c_PID->cd(9); gPad->SetLogz();
  h2_HMS_CalTrackCher_Uncut->Draw("COLZ");
  c_PID->cd(10); gPad->SetLogz();
  h2_HMS_CalTrackCher_Cut->Draw("COLZ");
  c_PID->Print(foutpdf);

  TCanvas *c_CT = new TCanvas("c_CT", "Proton CT distributions", 100, 0, 1000, 900);
  c_CT->Divide(3,2);
  c_CT->cd(1);
  h1_CT_All->Draw();
  c_CT->cd(2);
  h1_CT_Prompt->Draw();
  c_CT->cd(3);
  h1_CT_Random->Draw();
  c_CT->cd(4);
  h2_CT_Beta_All->Draw("COLZ");
  c_CT->cd(5);
  h2_CT_Beta_Prompt->Draw("COLZ");
  c_CT->cd(6);
  h2_CT_Beta_Random->Draw("COLZ");
  c_CT->Print(foutpdf);

  TCanvas *c_CT2 = new TCanvas("c_CT2", "Proton CT vs MM distributions", 100, 0, 1000, 900);
  c_CT2->Divide(1,3);
  c_CT2->cd(1);
  h2_CT_MMp_All->Draw("COLZ");
  c_CT2->cd(2);
  h2_CT_MMp_Prompt->Draw("COLZ");
  c_CT2->cd(3);
  h2_CT_MMp_Random->Draw("COLZ");
  c_CT2->Print(foutpdf);

  TCanvas *c_RFCut = new TCanvas("c_RFCut", "Proton RFCut distributions", 100, 0, 1000, 900);
  c_RFCut->Divide(2,2);
  c_RFCut->cd(1);
  h1_RFCutDist_woCut->Draw();
  c_RFCut->cd(2);
  h1_RFCutDist_woCut->SetLineColor(2);
  h1_RFCutDist_woCut->Draw();
  h1_RFCutDist_wCut->SetLineColor(4);
  h1_RFCutDist_wCut->Draw("SAME");
  c_RFCut->cd(3);
  h1_RFCutDist->SetLineColor(6);
  h1_RFCutDist->Draw();
  c_RFCut->cd(4);
  h1_RFCutDist->Draw();
  h1_RFCutDist_woCut->Draw("SAME");
  h1_RFCutDist_wCut->Draw("SAME");
  c_RFCut->Print(foutpdf);
   
  TCanvas *c_Kine = new TCanvas("c_Kine", "Kinematics info", 100, 0, 1000, 900);
  c_Kine->Divide(2,2);
  c_Kine->cd(1);
  h2_Q2vsW->Draw("COLZ");
  c_Kine->cd(2);
  h1_Epsilon->Draw();
  c_Kine->cd(3);
  h2_phiqvst->Draw("SURF2 POL"); 
  // Horrible block of stuff for polar plotting
  gPad->SetTheta(90); gPad->SetPhi(180);
  TPaveText *tvsphi_title = new TPaveText(0.0277092,0.89779,0.096428,0.991854,"NDC");
  tvsphi_title->AddText("-t vs #phi"); tvsphi_title->Draw();
  TPaveText *ptphizero = new TPaveText(0.923951,0.513932,0.993778,0.574551,"NDC");
  ptphizero->AddText("#phi = 0"); ptphizero->Draw();
  TLine *phihalfpi = new TLine(0,0,0,0.6); 
  phihalfpi->SetLineColor(kBlack); phihalfpi->SetLineWidth(2); phihalfpi->Draw();  
  TPaveText *ptphihalfpi = new TPaveText(0.417855,0.901876,0.486574,0.996358,"NDC");
  ptphihalfpi->AddText("#phi = #frac{#pi}{2}"); ptphihalfpi->Draw();
  TLine *phipi = new TLine(0,0,-0.6,0); 
  phipi->SetLineColor(kBlack); phipi->SetLineWidth(2); phipi->Draw();  
  TPaveText *ptphipi = new TPaveText(0.0277092,0.514217,0.096428,0.572746,"NDC");
  ptphipi->AddText("#phi = #pi"); ptphipi->Draw();
  TLine *phithreepi = new TLine(0,0,0,-0.6); 
  phithreepi->SetLineColor(kBlack); phithreepi->SetLineWidth(2); phithreepi->Draw();  
  TPaveText *ptphithreepi = new TPaveText(0.419517,0.00514928,0.487128,0.0996315,"NDC");
  ptphithreepi->AddText("#phi = #frac{3#pi}{2}"); ptphithreepi->Draw();
  TArc *Arc[10];
  for (Int_t k = 0; k < 10; k++){
    Arc[k] = new TArc(); 
    Arc[k]->SetFillStyle(0);
    Arc[k]->SetLineWidth(2);
    Arc[k]->DrawArc(0,0,0.575*(k+1)/(10),0.,360.,"same"); 
  }
  TGaxis *tradius = new TGaxis(0,0,0.575,0,0,2.0,10,"-+"); 
  tradius->SetLineColor(2);tradius->SetLabelColor(2);tradius->Draw();
  TLine *phizero = new TLine(0,0,0.6,0); 
  phizero->SetLineColor(kBlack); phizero->SetLineWidth(2); phizero->Draw();
  c_Kine->cd(4);
  h1_MMp_BGSub->Draw("HIST");
  //Proton_Fit->DrawClone("SAME");
  c_Kine->Print(foutpdf + ')');

  TFile *OutHisto_file = new TFile(foutname,"RECREATE");
  TDirectory *d_ProtonAll = OutHisto_file->mkdir("All Proton events, after cuts");
  TDirectory *d_ProtonPr = OutHisto_file->mkdir("Prompt Proton events, after cuts");
  TDirectory *d_ProtonRn = OutHisto_file->mkdir("Random Proton events, after cuts");
  TDirectory *d_ProtonRF = OutHisto_file->mkdir("Proton RF Info");
  TDirectory *d_ProtonTracking = OutHisto_file->mkdir("Proton tracking cut info");
  TDirectory *d_ProtonPID = OutHisto_file->mkdir("Proton PID cut info");
  TDirectory *d_Kine = OutHisto_file->mkdir("Proton kinematics info");
  
  d_ProtonAll->cd();
  h1_MMp_All->Write();
  h1_CT_All->Write();
  h2_CT_Beta_All->Write();
  h2_CT_MMp_All->Write();

  d_ProtonPr->cd();
  h1_MMp_Prompt->Write();
  h1_CT_Prompt->Write();
  h2_CT_Beta_Prompt->Write();
  h2_CT_MMp_Prompt->Write();
 
  d_ProtonRn->cd();
  h1_MMp_Random->Write();
  h1_CT_Random->Write();
  h2_CT_Beta_Random->Write();
  h2_CT_MMp_Random->Write();

  d_ProtonRF->cd();
  h1_RFCutDist->Write();
  h1_RFCutDist_woCut->Write();
  h1_RFCutDist_wCut->Write();

  d_Kine->cd();
  h2_Q2vsW->Write();
  h1_Epsilon->Write();
  h2_phiqvst->Write();
  h1_MMp_BGSub->Write();

  d_ProtonTracking->cd();
  h1_HDelta_Uncut->Write();
  h1_HDelta_Cut->Write();
  h1_PDelta_Uncut->Write();
  h1_PDelta_Cut->Write();
  h1_Hxp_Uncut->Write();
  h1_Hxp_Cut->Write();
  h1_Pxp_Uncut->Write();
  h1_Pxp_Cut->Write();
  h1_Hyp_Uncut->Write();
  h1_Hyp_Cut->Write();
  h1_Pyp_Uncut->Write();
  h1_Pyp_Cut->Write();

  d_ProtonPID->cd();
  h1_Aero_Uncut->Write();
  h1_Aero_Cut->Write();
  h1_HGC_Uncut->Write();
  h1_HGC_Cut->Write();
  h1_HCal_Uncut->Write();
  h1_HCalTrack_Uncut->Write();
  h1_HCal_Cut->Write();
  h1_HCalTrack_Cut->Write(); 
  h1_PCal_Uncut->Write();
  h1_PCalTrack_Uncut->Write();
  h1_PCal_Cut->Write();
  h1_PCalTrack_Cut->Write();
  h2_AeroHGC_Uncut->Write();
  h2_AeroHGC_Cut->Write();
  h2_HMS_CalCher_Uncut->Write();
  h2_HMS_CalCher_Cut->Write();
  h2_HMS_CalTrackCher_Uncut->Write();
  h2_HMS_CalTrackCher_Cut->Write();

  OutHisto_file->Close();

}
