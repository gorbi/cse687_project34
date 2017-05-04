///////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/
#include "Window.h"
using namespace CppCliWindows;

WPFCppCliDemo::WPFCppCliDemo()
{
  // set up channel
  ObjectFactory* pObjFact = new ObjectFactory;
  pSendr_ = pObjFact->createSendr();
  pRecvr_ = pObjFact->createRecvr();
  pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_);
  pChann_->start();
  delete pObjFact;
  // client's receive thread
  recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));
  recvThread->Start();
  // set event handlers
  this->Loaded += 
    gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
  this->Closing += 
    gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
  hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendMessage);
  hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
  hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
  hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::uploadFileList);
  hDisplayButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::displayFilesForCategory);
  hDownloadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadFilesForCategory);
  hDeleteButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::deleteCategory);
  // set Window properties
  this->Title = "Client";
  this->Width = 800;
  this->Height = 600;
  // attach dock panel to Window
  this->Content = hDockPanel;
  hDockPanel->Children->Add(hStatusBar);
  hDockPanel->SetDock(hStatusBar, Dock::Bottom);
  hDockPanel->Children->Add(hGrid);
  // setup Window controls and views
  setUpTabControl();
  setUpStatusBar();
  setUpSendMessageView();
  setUpFileListView();
  setUpConnectionView();
}

WPFCppCliDemo::~WPFCppCliDemo()
{
  delete pChann_;
  delete pSendr_;
  delete pRecvr_;
}

void WPFCppCliDemo::setUpStatusBar()
{
	grid->Children->Add(hRadioCategoryDFL2);
	RowDefinition^ hRow2Def2222q = gcnew RowDefinition();
	hRow2Def2222q->Height = GridLength(15);
	grid->RowDefinitions->Add(hRow2Def2222q);
	hRadioCategoryDFL3->GroupName = "Categories DF";
	hRadioCategoryDFL3->Content = "Category 3";
	hRadioCategoryDFL3->Height = 30;
	hRadioCategoryDFL3->Width = 120;
	hRadioCategoryDFL3->BorderThickness = Thickness(2);
	hRadioCategoryDFL3->BorderBrush = Brushes::Black;
	grid->SetRow(hRadioCategoryDFL3, 3);
	grid->Children->Add(hRadioCategoryDFL3);
	RowDefinition^ rd = gcnew RowDefinition();
	rd->Height = GridLength(35);
	grid->RowDefinitions->Add(rd);
	hDisplayButton->Content = "Display";
	hDisplayButton->Height = 30;
	hDisplayButton->Width = 120;
	hDisplayButton->BorderThickness = Thickness(2);
	hDisplayButton->BorderBrush = Brushes::Black;
	grid->SetRow(hDisplayButton, 4);
	grid->Children->Add(hDisplayButton);
	RowDefinition^ rd1 = gcnew RowDefinition();
	rd1->Height = GridLength(35);
	grid->RowDefinitions->Add(rd1);
	hDownloadButton->Content = "Download";
	hDownloadButton->Height = 30;
	hDownloadButton->Width = 120;
	hDownloadButton->BorderThickness = Thickness(2);
	hDownloadButton->BorderBrush = Brushes::Black;
	grid->SetRow(hDownloadButton, 5);
	grid->Children->Add(hDownloadButton);
}

void WPFCppCliDemo::setUpTabControl()
{
  hGrid->Children->Add(hTabControl);
  hSendMessageTab->Header = "Publish / Delete / Messages from server";
  hUploadFLTab->Header = "Upload File List";
  hDisplayFLTab->Header = "Display File List";
  hTabControl->Items->Add(hUploadFLTab);
  hTabControl->Items->Add(hDisplayFLTab);
  hTabControl->Items->Add(hSendMessageTab);
  hStatusBar->Items->Add(hStatusBarItem);
  hStatus->Text = "very important messages will appear here";
  //status->FontWeight = FontWeights::Bold;
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
  Console::Write("\n  setting up sendMessage view");
  hSendMessageGrid->Margin = Thickness(20);
  hSendMessageTab->Content = hSendMessageGrid;
  setTextBlockProperties();
  setButtonsProperties();
  grid->Margin = Thickness(20);
  hDisplayFLTab->Content = grid;
  RowDefinition^ hRow1Defq = gcnew RowDefinition();
  grid->RowDefinitions->Add(hRow1Defq);
  Grid^ hGridq = gcnew Grid();
  hGridq->SetColumn(grid, 0);
  hGridq->SetRow(grid, 0);
  hGridq->SetColumnSpan(grid, 2);
  hDListBox->SelectionMode = SelectionMode::Multiple;
  hGridq->Children->Add(hDListBox);
  grid->Children->Add(hGridq);
  Border^ borderq = gcnew Border();
  borderq->Margin = Thickness(20);
  grid->Children->Add(borderq);
  RowDefinition^ hRow2Def22q = gcnew RowDefinition();
  hRow2Def22q->Height = GridLength(15);
  grid->RowDefinitions->Add(hRow2Def22q);
  hRadioCategoryDFL1->GroupName = "Categories DF";
  hRadioCategoryDFL1->Content = "Category 1";
  hRadioCategoryDFL1->Height = 30;
  hRadioCategoryDFL1->Width = 120;
  hRadioCategoryDFL1->BorderThickness = Thickness(2);
  hRadioCategoryDFL1->BorderBrush = Brushes::Black;
  grid->SetRow(hRadioCategoryDFL1, 1);
  grid->Children->Add(hRadioCategoryDFL1);
  RowDefinition^ hRow2Def222q = gcnew RowDefinition();
  hRow2Def222q->Height = GridLength(15);
  grid->RowDefinitions->Add(hRow2Def222q);
  hRadioCategoryDFL2->GroupName = "Categories DF";
  hRadioCategoryDFL2->Content = "Category 2";
  hRadioCategoryDFL2->Height = 30;
  hRadioCategoryDFL2->Width = 120;
  hRadioCategoryDFL2->BorderThickness = Thickness(2);
  hRadioCategoryDFL2->BorderBrush = Brushes::Black;
  grid->SetRow(hRadioCategoryDFL2, 2);
}

void WPFCppCliDemo::setTextBlockProperties()
{
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hSendMessageGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hTextBlock1;
  hTextBlock1->Padding = Thickness(15);
  hTextBlock1->Text = "";
  hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
  hTextBlock1->FontWeight = FontWeights::Bold;
  hTextBlock1->FontSize = 16;
  hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
  hScrollViewer1->Content = hBorder1;
  hSendMessageGrid->SetRow(hScrollViewer1, 0);
  hSendMessageGrid->Children->Add(hScrollViewer1);
}

void WPFCppCliDemo::setButtonsProperties()
{
	RowDefinition^ hRow2Def22f = gcnew RowDefinition();
	hRow2Def22f->Height = GridLength(40);
	hSendMessageGrid->RowDefinitions->Add(hRow2Def22f);
	hClearButton->Content = "Clear";
	hClearButton->Height = 30;
	hClearButton->Width = 120;
	hClearButton->BorderThickness = Thickness(2);
	hClearButton->BorderBrush = Brushes::Black;
	hSendMessageGrid->SetRow(hClearButton, 1);
	hSendMessageGrid->Children->Add(hClearButton);
  RowDefinition^ hRow2Def22 = gcnew RowDefinition();
  hRow2Def22->Height = GridLength(15);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def22);
  hRadioCategoryPM1->GroupName = "Categories PM";
  hRadioCategoryPM1->Content = "Category 1";
  hRadioCategoryPM1->Height = 30;
  hRadioCategoryPM1->Width = 120;
  hRadioCategoryPM1->BorderThickness = Thickness(2);
  hRadioCategoryPM1->BorderBrush = Brushes::Black;
  hSendMessageGrid->SetRow(hRadioCategoryPM1, 2);
  hSendMessageGrid->Children->Add(hRadioCategoryPM1);
  RowDefinition^ hRow2Def22w = gcnew RowDefinition();
  hRow2Def22w->Height = GridLength(15);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def22w);
  hRadioCategoryPM2->GroupName = "Categories PM";
  hRadioCategoryPM2->Content = "Category 2";
  hRadioCategoryPM2->Height = 30;
  hRadioCategoryPM2->Width = 120;
  hRadioCategoryPM2->BorderThickness = Thickness(2);
  hRadioCategoryPM2->BorderBrush = Brushes::Black;
  hSendMessageGrid->SetRow(hRadioCategoryPM2, 3);
  hSendMessageGrid->Children->Add(hRadioCategoryPM2);
  RowDefinition^ hRow2Def22c = gcnew RowDefinition();
  hRow2Def22c->Height = GridLength(15);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def22c);
  hRadioCategoryPM3->GroupName = "Categories PM";
  hRadioCategoryPM3->Content = "Category 3";
  hRadioCategoryPM3->Height = 30;
  hRadioCategoryPM3->Width = 120;
  hRadioCategoryPM3->BorderThickness = Thickness(2);
  hRadioCategoryPM3->BorderBrush = Brushes::Black;
  hSendMessageGrid->SetRow(hRadioCategoryPM3, 4);
  hSendMessageGrid->Children->Add(hRadioCategoryPM3);
  RowDefinition^ hRow2Def22fa = gcnew RowDefinition();
  hRow2Def22fa->Height = GridLength(40);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def22fa);
  hSendButton->Content = "Publish";
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hBorder3->Child = hSendButton;
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  hDeleteButton->Content = "Delete";
  Border^ hBorder34 = gcnew Border();
  hBorder34->Width = 120;
  hBorder34->Height = 30;
  hBorder34->BorderThickness = Thickness(1);
  hBorder34->BorderBrush = Brushes::Black;
  hBorder34->Child = hDeleteButton;
  StackPanel^ hStackPanel112 = gcnew StackPanel();
  hStackPanel112->Children->Add(hBorder3);
  hStackPanel112->Children->Add(hSpacer);
  hStackPanel112->Children->Add(hBorder34);
  hStackPanel112->Orientation = Orientation::Horizontal;
  hStackPanel112->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hSendMessageGrid->SetRow(hStackPanel112, 5);
  hSendMessageGrid->Children->Add(hStackPanel112);
}

void WPFCppCliDemo::setUpSendMessageView()
{

}

std::string WPFCppCliDemo::toStdString(String^ pStr)
{
  std::string dst;
  for (int i = 0; i < pStr->Length; ++i)
    dst += (char)pStr[i];
  return dst;
}

void WPFCppCliDemo::sendMessage(Object^ obj, RoutedEventArgs^ args)
{
  int category = getCategoryPM();

  if (category == -1)
	  return;

  pSendr_->postMessage(toStdString("PUBLISH," + category));
  Console::Write("PUBLISHED " + category + "\n");
  hStatus->Text = "PUBLISHED " + category;
}

String^ WPFCppCliDemo::toSystemString(std::string& str)
{
  StringBuilder^ pStr = gcnew StringBuilder();
  for (size_t i = 0; i < str.size(); ++i)
    pStr->Append((Char)str[i]);
  return pStr->ToString();
}

void WPFCppCliDemo::addText(String^ msg)
{
  array<String^>^ array = msg->Split(',');
  if (array[0] == "PUBLISH" || array[0] == "DELETE" || array[0] == "UPLOAD" || array[0] == "DOWNLOAD") {
	  hTextBlock1->Text += array[1] + "\n";
  }
  else if (array[0] == "DISPLAY") {
	  hDListBox->Items->Clear();
	  if (array->Length > 2) {
		  for (int i = 2; i < array->Length; i++) {
			  hDListBox->Items->Add(array[i]);
		  }
	  }
	  hTextBlock1->Text += array[1] + "\n";
  }
}

void WPFCppCliDemo::getMessage()
{
  // recvThread runs this function

  while (true)
  {
    std::cout << "\n  receive thread calling getMessage()";
    std::string msg = pRecvr_->getMessage();
    String^ sMsg = toSystemString(msg);
    array<String^>^ args = gcnew array<String^>(1);
    args[0] = sMsg;

    Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
    Dispatcher->Invoke(act, args);  // must call addText on main UI thread
  }
}

void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  cleared message text");
  hStatus->Text = "Cleared message";
  hTextBlock1->Text = "";
}


void WPFCppCliDemo::uploadFileList(Object^ sender, RoutedEventArgs^ args)
{
	int category = getCategoryUFL();

	if (category == -1 || hListBox->Items->Count <= 0)
		return;

	std::cout << toStdString("\n invoked upload for category: " + category);
	String^ files;
	for each (String^ item in hListBox->Items) {
		files += item+",";
	}
	files += "END";

	pSendr_->postMessage(toStdString("UPLOAD," + category+ "," + files));

	Console::Write("UPLOAD TO " + category + "\n");
	hStatus->Text = "UPLOAD TO " + category;
}


void WPFCppCliDemo::setUpFileListView()
{
  Console::Write("\n  setting up FileList view");
  hFileListGrid->Margin = Thickness(20);
  hUploadFLTab->Content = hFileListGrid;
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  //hRow1Def->Height = GridLength(75);
  hFileListGrid->RowDefinitions->Add(hRow1Def);
  Grid^ hGrid = gcnew Grid();
  hGrid->SetColumn(hFileListGrid, 0);
  hGrid->SetRow(hFileListGrid, 0);
  hGrid->SetColumnSpan(hFileListGrid, 2);
  hListBox->SelectionMode = SelectionMode::Multiple;
  hGrid->Children->Add(hListBox);
  hFileListGrid->Children->Add(hGrid);
  Border^ border = gcnew Border();
  border->Margin = Thickness(20);
  hFileListGrid->Children->Add(border);
  //Category 1
  RowDefinition^ hRow2Def22 = gcnew RowDefinition();
  hRow2Def22->Height = GridLength(15);
  hFileListGrid->RowDefinitions->Add(hRow2Def22);
  hRadioCategoryUFL1->GroupName = "Categories";
  hRadioCategoryUFL1->Content = "Category 1";
  hRadioCategoryUFL1->Height = 30;
  hRadioCategoryUFL1->Width = 120;
  hRadioCategoryUFL1->BorderThickness = Thickness(2);
  hRadioCategoryUFL1->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hRadioCategoryUFL1, 1);
  hFileListGrid->Children->Add(hRadioCategoryUFL1);
  //Category 2
  RowDefinition^ hRow2Def222 = gcnew RowDefinition();
  hRow2Def222->Height = GridLength(15);
  hFileListGrid->RowDefinitions->Add(hRow2Def222);
  hRadioCategoryUFL2->GroupName = "Categories";
  hRadioCategoryUFL2->Content = "Category 2";
  hRadioCategoryUFL2->Height = 30;
  hRadioCategoryUFL2->Width = 120;
  hRadioCategoryUFL2->BorderThickness = Thickness(2);
  hRadioCategoryUFL2->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hRadioCategoryUFL2, 2);
  hFileListGrid->Children->Add(hRadioCategoryUFL2);
  
}


void WPFCppCliDemo::directorySearch(String^ dir) {
	array<String^>^ filesH = System::IO::Directory::GetFiles(dir, L"*.h");
	for (int i = 0; i < filesH->Length; ++i)
		hListBox->Items->Add(filesH[i]);
	array<String^>^ filesCPP = System::IO::Directory::GetFiles(dir, L"*.cpp");
	for (int i = 0; i < filesCPP->Length; ++i)
		hListBox->Items->Add(filesCPP[i]);

	array<String^>^ dirs = System::IO::Directory::GetDirectories(dir);
	for (int i = 0; i < dirs->Length; ++i)
		directorySearch(dirs[i]);
}

int CppCliWindows::WPFCppCliDemo::getCategoryUFL()
{
	if (hRadioCategoryUFL1->IsChecked) {
		return 1;
	}
	else if (hRadioCategoryUFL2->IsChecked) {
		return 2;
	}
	else if (hRadioCategoryUFL3->IsChecked) {
		return 3;
	}
	else {
		return -1;
	}
}

int CppCliWindows::WPFCppCliDemo::getCategoryDFL()
{
	if (hRadioCategoryDFL1->IsChecked) {
		return 1;
	}
	else if (hRadioCategoryDFL2->IsChecked) {
		return 2;
	}
	else if (hRadioCategoryDFL3->IsChecked) {
		return 3;
	}
	else {
		return -1;
	}
}

int CppCliWindows::WPFCppCliDemo::getCategoryPM()
{
	if (hRadioCategoryPM1->IsChecked) {
		return 1;
	}
	else if (hRadioCategoryPM2->IsChecked) {
		return 2;
	}
	else if (hRadioCategoryPM3->IsChecked) {
		return 3;
	}
	else {
		return -1;
	}
}

void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  std::cout << "\n  Browsing for folder";
  hListBox->Items->Clear();
  System::Windows::Forms::DialogResult result;
  result = hFolderBrowserDialog->ShowDialog();
  if (result == System::Windows::Forms::DialogResult::OK)
  {
    String^ path = hFolderBrowserDialog->SelectedPath;
    std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
	directorySearch(path);
  }
}
void CppCliWindows::WPFCppCliDemo::displayFilesForCategory(Object ^ sender, RoutedEventArgs ^ args)
{
	int category = getCategoryDFL();

	if (category == -1)
		return;

	pSendr_->postMessage(toStdString("DISPLAY," + category));
	Console::Write("DISPLAY " + category + "\n");
	hStatus->Text = "DISPLAY " + category;

	std::cout << toStdString("\n invoked display for category: " + category);
}
void CppCliWindows::WPFCppCliDemo::downloadFilesForCategory(Object ^ sender, RoutedEventArgs ^ args)
{
	int category = getCategoryDFL();

	if (category == -1)
		return;

	std::cout << toStdString("\n invoked download for category: " + category);
	String^ files;
	for each (String^ item in hDListBox->SelectedItems) {
		files += item + ",";
		std::cout << toStdString("\n downloading file: " + item);
	}
	if (hDListBox->SelectedItems->Count > 0) {
		files += "END";
		pSendr_->postMessage(toStdString("DOWNLOAD," + category + "," + files));
	}
	else {
		pSendr_->postMessage(toStdString("DOWNLOAD," + category + ",ALL"));
	}

	Console::Write("DOWNLOAD FROM " + category + "\n");
	hStatus->Text = "DOWNLOAD FROM " + category;

}
void CppCliWindows::WPFCppCliDemo::deleteCategory(Object ^ sender, RoutedEventArgs ^ args)
{
	int category = getCategoryPM();

	if (category == -1)
		return;

	pSendr_->postMessage(toStdString("DELETE," + category));
	Console::Write("DELETE " + category+"\n");
	hStatus->Text = "DELETE " + category;
	std::cout << toStdString("\n invoked delete for category: " + category);
}
void WPFCppCliDemo::setUpConnectionView()
{
  Console::Write("\n  setting up Connection view");
  //Category 3
  RowDefinition^ hRow2Def2222 = gcnew RowDefinition();
  hRow2Def2222->Height = GridLength(15);
  hFileListGrid->RowDefinitions->Add(hRow2Def2222);
  hRadioCategoryUFL3->GroupName = "Categories";
  hRadioCategoryUFL3->Content = "Category 3";
  hRadioCategoryUFL3->Height = 30;
  hRadioCategoryUFL3->Width = 120;
  hRadioCategoryUFL3->BorderThickness = Thickness(2);
  hRadioCategoryUFL3->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hRadioCategoryUFL3, 3);
  hFileListGrid->Children->Add(hRadioCategoryUFL3);
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(35);
  hFileListGrid->RowDefinitions->Add(hRow2Def);
  hFolderBrowseButton->Content = "Select Directory";
  hFolderBrowseButton->Height = 30;
  hFolderBrowseButton->Width = 120;
  hFolderBrowseButton->BorderThickness = Thickness(2);
  hFolderBrowseButton->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hFolderBrowseButton, 4);
  hFileListGrid->Children->Add(hFolderBrowseButton);
  // Show selected items button.
  RowDefinition^ hRow2Def2 = gcnew RowDefinition();
  hRow2Def2->Height = GridLength(35);
  hFileListGrid->RowDefinitions->Add(hRow2Def2);
  hUploadButton->Content = "Upload";
  hUploadButton->Height = 30;
  hUploadButton->Width = 120;
  hUploadButton->BorderThickness = Thickness(2);
  hUploadButton->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hUploadButton, 5);
  hFileListGrid->Children->Add(hUploadButton);
  hFolderBrowserDialog->ShowNewFolderButton = false;
  hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  Window loaded");
}
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
  Console::WriteLine(L"\n Starting WPFCppCliDemo");

  Application^ app = gcnew Application();
  app->Run(gcnew WPFCppCliDemo());
  Console::WriteLine(L"\n\n");
}