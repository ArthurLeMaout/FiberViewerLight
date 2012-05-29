#include "FiberViewerLightGUI.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkPolyDataReader.h"


/********************************************************************************
 *Constructor: Initialize widgets and renderer and connect slots and signals
 ********************************************************************************/

FiberViewerLightGUI::FiberViewerLightGUI(bool nogui, std::string input, std::string output, QWidget* parent):QWidget(parent)
{
	m_NoGUI=nogui;
	InitWidgets();
	if(!nogui)
	{
		connect(m_LE_VTKInput, SIGNAL(editingFinished()), this, SLOT(EnterVTKInput()));
		connect(m_TB_BrowserVTKInput, SIGNAL(clicked()), this, SLOT(BrowserVTKInput()));
		connect(m_LE_OutputFolder,SIGNAL(editingFinished()),this,SLOT(EnterOutputFolder()));
		connect(m_TB_BrowserOutput, SIGNAL(clicked()), this, SLOT(BrowserOutput()));
		connect(m_PB_Length, SIGNAL(clicked()), this, SLOT(OpenLengthPanel()));
		connect(m_PB_Gravity, SIGNAL(clicked()), this, SLOT(OpenDistributionPanel()));
		connect(m_PB_Hausdorff, SIGNAL(clicked()), this, SLOT(OpenDistributionPanel()));
		connect(m_PB_Mean, SIGNAL(clicked()), this, SLOT(OpenDistributionPanel()));
		connect(m_PB_NormCut, SIGNAL(clicked()), this, SLOT(OpenNormCutPanel()));
		connect(m_PB_Cutter, SIGNAL(clicked()),this,SLOT(OpenCutterPanel()));
		connect(m_LengthGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseLengthPanel(FVPanelGUI::ExitSignal)));
		connect(m_DistributionGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseDistributionPanel(FVPanelGUI::ExitSignal)));
		connect(m_DisplayClassGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseDisplayClass(FVPanelGUI::ExitSignal)));
		connect(m_NormCutGUI, SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseNormCut(FVPanelGUI::ExitSignal)));
		connect(m_CutterGUI,SIGNAL(Exit(FVPanelGUI::ExitSignal)), this, SLOT(CloseCutter(FVPanelGUI::ExitSignal)));
		connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
		connect(m_PB_Redo, SIGNAL(clicked()), this, SLOT(RedoAction()));
		connect(m_PB_SaveVTK, SIGNAL(clicked()), this, SLOT(SaveVTK()));
		connect(m_PB_Plane, SIGNAL(clicked()), this, SLOT(OpenPlanSetting()));
		connect(m_DistributionGUI, SIGNAL(Progress(int)), m_ProgressBar, SLOT(setValue(int)));
		connect(m_LengthGUI, SIGNAL(Progress(int)), m_ProgressBar, SLOT(setValue(int)));
		connect(m_NormCutGUI, SIGNAL(Progress(int)), m_ProgressBar, SLOT(setValue(int)));
		connect(m_Display, SIGNAL(Progress(int)), m_ProgressBar, SLOT(setValue(int)));
		connect(m_S_PercentageDisplayed, SIGNAL(valueChanged(int)), this, SLOT(UpdatePercentage(int)));
		connect(m_PB_ApplyPercentage, SIGNAL(clicked()), this, SLOT(UpdateDisplayedLabel()));
		connect(m_Display, SIGNAL(NbFibersChanged(int)), this, SLOT(UpdateNbFibers(int)));
		connect(m_LE_NbVoxelX, SIGNAL(editingFinished()), this, SLOT(UpdateSpacing()));
		connect(m_LE_NbVoxelY, SIGNAL(editingFinished()), this, SLOT(UpdateSpacing()));
		connect(m_LE_NbVoxelZ, SIGNAL(editingFinished()), this, SLOT(UpdateSpacing()));
		connect(m_RB_DT,SIGNAL(clicked()), this, SLOT(EnableVoxels()));
		connect(m_RB_Classic,SIGNAL(clicked()), this, SLOT(EnableVoxels()));
		connect(m_RB_Save,SIGNAL(clicked()), this, SLOT(EnableProcessingOption()));
		connect(m_RB_Load,SIGNAL(clicked()), this, SLOT(EnableProcessingOption()));
		
		m_LE_VTKInput->setText(input.c_str());
		m_LE_OutputFolder->setText(output.c_str());
		EnterVTKInput();
		EnterOutputFolder();
	}
}


/********************************************************************************
 *InitWidgets: Initialize all graphic widgets
 ********************************************************************************/

void FiberViewerLightGUI::InitWidgets()
{
	m_Display=new FiberDisplay(this);
	m_DistributionGUI=new FVDistributionGUI(this, m_Display);
	m_NormCutGUI=new FVNormCutGUI(this,m_Display);
	if(!m_NoGUI)
	{
		m_LengthGUI=new FVLengthGUI(this, m_Display);
		m_DisplayClassGUI=new FVDisplayClassGUI(this,m_Display);
		m_CutterGUI=new FVCutterGUI(this,m_Display);
		m_PlanSetting=new PlanSetting(this, m_Display);
		m_PlanSetting->hide();
		
		m_GB_ActionPanel=new QGroupBox("Fiber Viewer Light 1.0");
		m_GB_ActionPanel->setMinimumSize(350,620);
		m_GB_ActionPanel->setMaximumSize(350,620);
		
		m_GB_LengthPanel=new QGroupBox("Length");
		m_GB_LengthPanel->setMinimumSize(350,480);
		m_GB_LengthPanel->setMaximumSize(350,480);
		m_GB_LengthPanel->hide();
		m_GB_DistributionPanel=new QGroupBox("Distribution");
		m_GB_DistributionPanel->setMinimumSize(350,550);
		m_GB_DistributionPanel->setMaximumSize(350,550);
		m_GB_DistributionPanel->hide();
		m_GB_DisplayClassPanel=new QGroupBox("Display Class");
		m_GB_DisplayClassPanel->setMinimumSize(350,430);
		m_GB_DisplayClassPanel->setMaximumSize(350,430);
		m_GB_DisplayClassPanel->hide();
		m_GB_NormCutPanel=new QGroupBox("Normalized Cut");
		m_GB_NormCutPanel->setMinimumSize(350,430);
		m_GB_NormCutPanel->setMaximumSize(350,430);
		m_GB_NormCutPanel->hide();
		m_GB_CutterPanel=new QGroupBox("Cutter");
		m_GB_CutterPanel->setMinimumSize(350,430);
		m_GB_CutterPanel->setMaximumSize(350,430);
		m_GB_CutterPanel->hide();
			
		m_ProgressBar=new QProgressBar;
		m_ProgressBar->setValue(0);
		m_LE_VTKInput=new QLineEdit(this);
		m_LE_VTKInput->setMaximumHeight(20);
		m_LE_OutputFolder=new QLineEdit(this);
		m_LE_OutputFolder->setMaximumHeight(20);
		m_LE_NbVoxelX=new QLineEdit(this);
		m_LE_NbVoxelX->setEnabled(false);
		m_LE_NbVoxelX->setMaximumHeight(20);
		m_LE_NbVoxelY=new QLineEdit(this);
		m_LE_NbVoxelY->setEnabled(false);
		m_LE_NbVoxelY->setMaximumHeight(20);
		m_LE_NbVoxelZ=new QLineEdit(this);
		m_LE_NbVoxelZ->setEnabled(false);
		m_LE_NbVoxelZ->setMaximumHeight(20);
		m_TB_BrowserVTKInput=new QToolButton(this);
		m_TB_BrowserVTKInput->setText("...");
		m_TB_BrowserVTKInput->setMaximumHeight(20);
		m_TB_BrowserOutput=new QToolButton(this);
		m_TB_BrowserOutput->setText("...");
		m_TB_BrowserOutput->setMaximumHeight(20);
		m_L_VTKInput=new QLabel("VTK Input", this);
		m_L_OutputFolder=new QLabel("Output Folder", this);
		m_L_NbVoxel=new QLabel("Nb Voxels", this);
		m_L_NbVoxelX=new QLabel("X", this);
		m_L_NbVoxelY=new QLabel("Y", this);
		m_L_NbVoxelZ=new QLabel("Z", this);
		m_L_IO=new QLabel("Select input/output", this);
		m_L_IO->setFrameShadow(QFrame::Plain);
		m_L_Processing=new QLabel("Processing", this);
		m_L_Processing->setFrameShadow(QFrame::Plain);
		m_L_ProcessingOption=new QLabel("Processing Options", this);
		m_L_ProcessingOption->setFrameShadow(QFrame::Plain);
		m_L_Navigation=new QLabel("Navigation", this);
		m_L_Navigation->setFrameShadow(QFrame::Plain);
		m_PB_Length=new QPushButton("Length",this);
		m_PB_Gravity=new QPushButton("Gravity",this);
		m_PB_Hausdorff=new QPushButton("Hausdorff",this);
		m_PB_Mean=new QPushButton("Mean",this);
		m_PB_NormCut=new QPushButton("Normalized Cut",this);
		m_PB_Cutter=new QPushButton("Cutter",this);
		m_PB_Undo=new QPushButton("Undo",this);
		m_PB_Undo->setEnabled(false);
		m_PB_Redo=new QPushButton("Redo",this);
		m_PB_Redo->setEnabled(false);
		m_PB_SaveVTK=new QPushButton("Save VTK File",this);
		m_PB_SaveVTK->setEnabled(false);
		m_PB_Plane=new QPushButton("Plane Option", this);
		m_PB_Plane->setEnabled(false);
		QButtonGroup* BG_DistanceOption=new QButtonGroup(this);
		m_RB_Save=new QRadioButton("Compute and Save\ndistance table",this);
		m_RB_Save->setEnabled(false);
		m_RB_Save->setChecked(true);
		BG_DistanceOption->addButton(m_RB_Save);
		m_RB_Load=new QRadioButton("Load distance table",this);
		m_RB_Load->setEnabled(false);
		BG_DistanceOption->addButton(m_RB_Load);
		QButtonGroup* BG_ProcessingType=new QButtonGroup(this);
		m_RB_Classic=new QRadioButton("Classic", this);
		m_RB_Classic->setEnabled(false);
		m_RB_Classic->setChecked(true);
		BG_ProcessingType->addButton(m_RB_Classic);
		m_RB_DT=new QRadioButton("Danielsson", this);
		m_RB_DT->setEnabled(false);
		BG_ProcessingType->addButton(m_RB_DT);
		
		
		m_L_NbFiber=new QLabel("No Fiber Loaded",this);
		m_PB_ApplyPercentage=new QPushButton("Apply",this);
		m_PB_ApplyPercentage->setEnabled(false);
		m_S_PercentageDisplayed=new QSlider(Qt::Horizontal, this);
		m_S_PercentageDisplayed->setMinimumWidth(200);
		m_S_PercentageDisplayed->setRange(1,10);
		m_S_PercentageDisplayed->setEnabled(false);
		m_L_PercentageDisplayed=new QLabel("", this);
		m_L_NbFiberDisplayed=new QLabel("", this);
		
		QFrame* F_HLine1=new QFrame;
		F_HLine1->setFrameShadow(QFrame::Plain);
		F_HLine1->setFrameShape(QFrame::HLine);
		QFrame* F_HLine2=new QFrame;
		F_HLine2->setFrameShadow(QFrame::Plain);
		F_HLine2->setFrameShape(QFrame::HLine);
		QFrame* F_HLine3=new QFrame;
		F_HLine3->setFrameShadow(QFrame::Plain);
		F_HLine3->setFrameShape(QFrame::HLine);
		QFrame* F_HLine4=new QFrame;
		F_HLine4->setFrameShadow(QFrame::Plain);
		F_HLine4->setFrameShape(QFrame::HLine);
		
		QGridLayout* GL_MainLayout=new QGridLayout;
		QVBoxLayout* VL_ActionPanel=new QVBoxLayout;
		QGridLayout* GL_FiberIO=new QGridLayout;
		QHBoxLayout* HL_Navigation=new QHBoxLayout;
		QGridLayout* GL_FiberInfo=new QGridLayout;
		QHBoxLayout* HL_ProcessingRB=new QHBoxLayout;
		QVBoxLayout* VL_DistanceOption=new QVBoxLayout;
		QVBoxLayout* VL_ProcessingType=new QVBoxLayout;
		QGridLayout* GL_Voxels=new QGridLayout;
		
		//Layout settings
		VL_ActionPanel->addWidget(m_L_IO);
		VL_ActionPanel->addWidget(F_HLine1);
		GL_FiberIO->addWidget(m_L_VTKInput,0,0);
		GL_FiberIO->addWidget(m_LE_VTKInput,0,1);
		GL_FiberIO->addWidget(m_TB_BrowserVTKInput,0,2);
		GL_FiberIO->addWidget(m_L_OutputFolder,1,0);
		GL_FiberIO->addWidget(m_LE_OutputFolder,1,1);
		GL_FiberIO->addWidget(m_TB_BrowserOutput,1,2);
		VL_ActionPanel->addLayout(GL_FiberIO);
		VL_ActionPanel->addWidget(m_L_Processing);
		VL_ActionPanel->addWidget(F_HLine2);
		VL_ActionPanel->addWidget(m_PB_Length);
		VL_ActionPanel->addWidget(m_PB_Gravity);
		VL_ActionPanel->addWidget(m_PB_Hausdorff);
		VL_ActionPanel->addWidget(m_PB_Mean);
		VL_ActionPanel->addWidget(m_PB_NormCut);
		VL_ActionPanel->addWidget(m_PB_Cutter);
		VL_ActionPanel->addWidget(m_L_ProcessingOption);
		VL_ActionPanel->addWidget(F_HLine3);
		VL_DistanceOption->addWidget(m_RB_Save);
		VL_DistanceOption->addWidget(m_RB_Load);
		HL_ProcessingRB->addLayout(VL_DistanceOption);
		VL_ProcessingType->addWidget(m_RB_Classic);
		VL_ProcessingType->addWidget(m_RB_DT);
		HL_ProcessingRB->addLayout(VL_ProcessingType);
		VL_ActionPanel->addLayout(HL_ProcessingRB);
		GL_Voxels->addWidget(m_L_NbVoxel,1,0,Qt::AlignHCenter);
		GL_Voxels->addWidget(m_L_NbVoxelX,0,1,Qt::AlignHCenter);
		GL_Voxels->addWidget(m_L_NbVoxelY,0,2,Qt::AlignHCenter);
		GL_Voxels->addWidget(m_L_NbVoxelZ,0,3,Qt::AlignHCenter);
		GL_Voxels->addWidget(m_LE_NbVoxelX,1,1);
		GL_Voxels->addWidget(m_LE_NbVoxelY,1,2);
		GL_Voxels->addWidget(m_LE_NbVoxelZ,1,3);
		VL_ActionPanel->addLayout(GL_Voxels);
		VL_ActionPanel->addWidget(m_L_Navigation);
		VL_ActionPanel->addWidget(F_HLine4);
		HL_Navigation->addWidget(m_PB_Undo);
		HL_Navigation->addWidget(m_PB_Redo);
		VL_ActionPanel->addLayout(HL_Navigation);
		VL_ActionPanel->addWidget(m_PB_SaveVTK);
		VL_ActionPanel->addWidget(m_ProgressBar);
		VL_ActionPanel->addStretch(1);
		
		m_GB_ActionPanel->setLayout(VL_ActionPanel);
		GL_MainLayout->addWidget(m_GB_ActionPanel,0,0,Qt::AlignTop);
	
		m_GB_LengthPanel->setLayout(m_LengthGUI->layout());
		GL_MainLayout->addWidget(m_GB_LengthPanel,0,0,Qt::AlignTop);
		m_GB_DistributionPanel->setLayout(m_DistributionGUI->layout());
		GL_MainLayout->addWidget(m_GB_DistributionPanel,0,0,Qt::AlignTop);
		m_GB_DisplayClassPanel->setLayout(m_DisplayClassGUI->layout());
		GL_MainLayout->addWidget(m_GB_DisplayClassPanel,0,0,Qt::AlignTop);
		m_GB_NormCutPanel->setLayout(m_NormCutGUI->layout());
		GL_MainLayout->addWidget(m_GB_NormCutPanel,0,0,Qt::AlignTop);
		m_GB_CutterPanel->setLayout(m_CutterGUI->layout());
		GL_MainLayout->addWidget(m_GB_CutterPanel,0,0,Qt::AlignTop);
		GL_MainLayout->addWidget(m_PlanSetting,1,0,Qt::AlignBottom);
		GL_MainLayout->setRowStretch(1,1);
		
		GL_FiberInfo->addWidget(m_L_NbFiber,0,0,Qt::AlignLeft);
		GL_FiberInfo->addWidget(m_L_NbFiberDisplayed,0,1,Qt::AlignLeft);
		GL_FiberInfo->addWidget(m_L_PercentageDisplayed,1,0,Qt::AlignRight);
		GL_FiberInfo->addWidget(m_S_PercentageDisplayed,1,1,Qt::AlignLeft);
		GL_FiberInfo->addWidget(m_PB_ApplyPercentage,2,0,Qt::AlignLeft);
		
		GL_MainLayout->addLayout(GL_FiberInfo,2,0,2,1);
		GL_MainLayout->addWidget(m_PB_Plane,3,1,Qt::AlignLeft);
		GL_MainLayout->addWidget(m_Display,0,1,3,1);
		GL_MainLayout->setColumnStretch(3,1);
		setLayout(GL_MainLayout);
	}
}

void FiberViewerLightGUI::EnableVoxels()
{
	if(sender()==m_RB_DT)
	{
		m_LE_NbVoxelX->setEnabled(true);
	}
	else
	{
		m_LE_NbVoxelX->setEnabled(false);
	}
}

void FiberViewerLightGUI::EnableProcessingOption()
{
	if(sender()==m_RB_Save)
	{
		m_RB_DT->setEnabled(true);
		m_RB_Classic->setEnabled(true);
	}
	else
	{
		m_RB_DT->setEnabled(false);
		m_RB_Classic->setEnabled(false);
		m_LE_NbVoxelX->setEnabled(false);
	}
}

int FiberViewerLightGUI::GetNbFiberDisplayed()
{
	return atoi(m_L_NbFiberDisplayed->text().toStdString().c_str());
}

void FiberViewerLightGUI::UpdatePercentage(int value)
{
	std::ostringstream oss;
	QString Percentage;
	oss<<value*10;
	Percentage=oss.str().c_str();
	m_L_PercentageDisplayed->setText(Percentage+"%");
}

void FiberViewerLightGUI::UpdateNbFibers(int value)
{
	std::ostringstream oss;
	QString NbFiber;
	oss<<value;
	NbFiber=oss.str().c_str();
	m_L_NbFiber->setText(NbFiber+" fiber(s)");
	UpdateDisplayedLabel();
}

void FiberViewerLightGUI::UpdateDisplayedLabel()
{
	std::ostringstream oss;
	QString Displayed;
	oss<<atoi(m_L_NbFiber->text().toStdString().c_str())*atoi(m_L_PercentageDisplayed->text().toStdString().c_str())/100;
	Displayed=oss.str().c_str();
	m_L_NbFiberDisplayed->setText(Displayed+" displayed");
	m_Display->SetNbFibersDisplayed(atoi(m_L_NbFiberDisplayed->text().toStdString().c_str()));
	m_Display->UpdateCells();
}

void FiberViewerLightGUI::UpdateSpacing()
{
	double Spacing, Bounds[6];
	int Voxels;
	Voxels=atoi(m_LE_NbVoxelX->text().toStdString().c_str());
	m_Display->GetBounds(Bounds);
	Spacing=ceil(Bounds[1]-Bounds[0]+1)/Voxels;
	m_Display->SetSpacing(Spacing);
	
	std::ostringstream oss;
	oss<<int(ceil(Bounds[3]-Bounds[2]+1)/Spacing);
	m_LE_NbVoxelY->setText(oss.str().c_str());
	
	oss.str("");
	oss<<int(ceil(Bounds[5]-Bounds[4]+1)/Spacing);
	m_LE_NbVoxelZ->setText(oss.str().c_str());
}

/********************************************************************************
 *BrowserVTKInput: Browser for input VTK file
 ********************************************************************************/

void FiberViewerLightGUI::BrowserVTKInput()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString filename,type;
	QFileDialog FileDialog;
	FileDialog.setViewMode(QFileDialog::Detail);
	filename = FileDialog.getOpenFileName(this, "Open VTK File", "/", "VTK (*.vtk *.vtp)",&type);
	
	m_LE_VTKInput->setText(filename);
	EnterVTKInput(true);
	
	QApplication::restoreOverrideCursor();	
}

/********************************************************************************
 *EnterVTKInput: If the VTK file entered is correctly loaded, initialize m_Fibers
 *	the renderer and m_Length table.
 ********************************************************************************/

void FiberViewerLightGUI::EnterVTKInput(bool FromBrowser)
{
	std::ostringstream nbfiber;
	if(m_LE_VTKInput->text().toStdString()!="")
	{
		vtkSmartPointer<vtkPolyData> PolyData;
		m_PreviousVtkFileName=m_VtkFileName;
		if(m_PreviousVtkFileName!=m_LE_VTKInput->text().toStdString() || FromBrowser)
			PolyData=LoadVTK(m_LE_VTKInput->text().toStdString());
		if(PolyData!=NULL)
		{
			m_VtkFileName=m_LE_VTKInput->text().toStdString();
			InitRedMap(PolyData);
			m_Display->InitRenderer();
			m_Display->StartRenderer(PolyData);
			
			double Bounds[6];
			m_Display->GetBounds(Bounds);
			std::ostringstream oss;
			oss<<ceil(Bounds[1]-Bounds[0]+1);
			m_LE_NbVoxelX->setText(oss.str().c_str());
			oss.str("");
			oss<<ceil(Bounds[3]-Bounds[2]+1);
			m_LE_NbVoxelY->setText(oss.str().c_str());
			oss.str("");
			oss<<ceil(Bounds[5]-Bounds[4]+1);
			m_LE_NbVoxelZ->setText(oss.str().c_str());
			
			m_Display->GetActor()->GetMapper()->SetScalarRange(0, PolyData->GetNumberOfCells()-1);
			m_Display->SetLookupTable(m_RedMap);
			m_PB_SaveVTK->setEnabled(true);
			m_PB_Plane->setEnabled(true);
			nbfiber<<PolyData->GetNumberOfCells();
			QString NbFiber=nbfiber.str().c_str();
			m_L_NbFiber->setText(NbFiber + " fiber(s)");
			if(PolyData->GetNumberOfCells()>20000)
			{
				if(m_S_PercentageDisplayed->value()!=200000/PolyData->GetNumberOfCells())
					m_S_PercentageDisplayed->setValue(200000/PolyData->GetNumberOfCells());
				else
					UpdatePercentage(m_S_PercentageDisplayed->value());
			}
			else
				m_S_PercentageDisplayed->setValue(10);
			UpdateDisplayedLabel();
			m_PB_ApplyPercentage->setEnabled(true);
			m_S_PercentageDisplayed->setEnabled(true);
			m_RB_Save->setEnabled(true);
			m_RB_Load->setEnabled(true);
			m_RB_DT->setEnabled(true);
			m_RB_Classic->setEnabled(true);
			m_Display->InitDTVector();
		}
	}
}

/********************************************************************************
 *BrowserVTKOutput: Browser for output VTK file
 ********************************************************************************/

void FiberViewerLightGUI::BrowserOutput()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString path = QFileDialog::getExistingDirectory(this);
	
	if(path!=NULL)
	{
		m_LE_OutputFolder->setText(path);
		EnterOutputFolder();
	}
	
	QApplication::restoreOverrideCursor();	
}


void FiberViewerLightGUI::EnterOutputFolder()
{
	m_OutputFolder=m_LE_OutputFolder->text().toStdString();
}
/********************************************************************************
 *LoadVTK: Load a VTK file and fill m_PolyData.
 ********************************************************************************/

vtkSmartPointer<vtkPolyData> FiberViewerLightGUI::LoadVTK(std::string FileName)
{
	std::cout<<"Reading VTK data..."<<std::endl;
	vtkSmartPointer<vtkPolyData> PolyData;
	if (FileName.rfind(".vtk") != std::string::npos)
	{
		vtkSmartPointer<vtkPolyDataReader> reader = vtkPolyDataReader::New();
		reader->SetFileName(FileName.c_str());
		PolyData=reader->GetOutput();
		reader->Update();
	}
	else if (FileName.rfind(".vtp") != std::string::npos)
	{
		vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkXMLPolyDataReader::New();
		reader->SetFileName(FileName.c_str());
		PolyData=reader->GetOutput();
		reader->Update();
	}
	else
	{
		std::cout<<"Error reading VTK File. Check VTK Format or file path."<<std::endl;
		return NULL;
	}
	std::cout<<"VTK File read successfuly. "<<PolyData->GetNumberOfCells()<<" fibers read."<<std::endl;
	return PolyData;
}

void FiberViewerLightGUI::InitRedMap(vtkPolyData* PolyData)
{
	//There is at most one color per fiber
	vtkLookupTable* RedMap=vtkLookupTable::New();
	RedMap->SetNumberOfTableValues(PolyData->GetNumberOfCells());
	
	vtkCellArray* Lines=PolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	vtkFloatArray* Colors=vtkFloatArray::New();
	
	//For each fiber
	Lines->InitTraversal();
	for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		//Fill Color array with R G B A (Alpha transparency)
		double Color[4]={0.85,0,0,1};
		
		//Fill the LUT
		RedMap->SetTableValue(i, Color);
		
		//Fill Scalars to be inserted in the PolyData
		for(int j=0; j<NumberOfPoints; j++)
			Colors->InsertNextValue(i);
	}
	PolyData->GetPointData()->SetScalars(Colors);
	m_RedMap=RedMap;
}

/********************************************************************************
 *BrowserSaveVTK: Option to save the actual VTK
 ********************************************************************************/

void FiberViewerLightGUI::SaveVTK()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString path;
	if(m_LE_OutputFolder->text()!="")
		path=m_LE_OutputFolder->text();
	else
		path=".";
	
	QString file = QFileDialog::getSaveFileName(this, "Save VTK file", path, "VTK (*.vtk)");
	
	QApplication::restoreOverrideCursor();	
	
	if(file!=NULL)
	{
		std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
		vtkSmartPointer<vtkPolyData> FinalPolyData=vtkSmartPointer<vtkPolyData>::New();
		
		vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
		
		vtkDataArray* Tensors=m_Display->GetOriginalPolyData()->GetPointData()->GetTensors();
		bool UseTensor=(Tensors!=NULL);
		vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
		NewTensors->SetNumberOfComponents(9);
	
		vtkPoints* Points=m_Display->GetOriginalPolyData()->GetPoints();
		vtkCellArray* Lines=m_Display->GetOriginalPolyData()->GetLines();
		vtkIdType* Ids;
		vtkIdType NumberOfPoints;
		int NewId=0;
		Lines->InitTraversal();
		for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
		{
			if(Alpha[i]==1)
			{
				vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New();
				NewLine->GetPointIds()->SetNumberOfIds(NumberOfPoints);
				for(int j=0; j<NumberOfPoints; j++)
				{
					NewPoints->InsertNextPoint(Points->GetPoint(Ids[j]));
					NewLine->GetPointIds()->SetId(j,NewId);
					NewId++;
					double tensorValue[9];
					if(UseTensor)
					{
						for(int k=0; k<9; k++)
							tensorValue[k]=Tensors->GetComponent(Ids[j],k);
						NewTensors->InsertNextTuple(tensorValue);
					}
				}
				NewLines->InsertNextCell(NewLine);
			}
		
		}
		FinalPolyData->SetPoints(NewPoints);
		if(UseTensor)
			FinalPolyData->GetPointData()->SetTensors(NewTensors);
		FinalPolyData->SetLines(NewLines);
		
		
		
		if (file.toStdString().rfind(".vtk") != std::string::npos)
		{
			vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
			fiberwriter->SetFileTypeToBinary();
			fiberwriter->SetFileName(file.toStdString().c_str());
			fiberwriter->SetInput(FinalPolyData);
			fiberwriter->Update();
		}
    // XML
		else if (file.toStdString().rfind(".vtp") != std::string::npos)
		{
			vtkSmartPointer<vtkXMLPolyDataWriter> fiberwriter = vtkXMLPolyDataWriter::New();
			fiberwriter->SetFileName(file.toStdString().c_str());
			fiberwriter->SetInput(FinalPolyData);
			fiberwriter->Update();
		}
	}
}


/********************************************************************************
 *OpenLengthPanel: 
 ********************************************************************************/

void FiberViewerLightGUI::OpenLengthPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		m_LengthGUI->InitLengthPanel();
		m_GB_ActionPanel->hide();
		m_GB_LengthPanel->show();
		m_ProgressBar->setValue(0);
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::CloseLengthPanel(FVPanelGUI::ExitSignal Type)
{
	m_GB_LengthPanel->hide();
	m_GB_ActionPanel->show();
	
	m_Display->UpdateCells();
	m_Display->SetLookupTable(m_RedMap);
	m_Display->Render();
	
	if(Type==FVPanelGUI::Ok)
	{
		m_PB_Redo->setEnabled(false);
		if(m_Display->AlphasIsUnchanged())
		{
			if(m_Display->GetAlphasSize(FiberDisplay::Previous)>1)
				m_PB_Undo->setEnabled(true);
			else
				m_PB_Undo->setEnabled(false);
		}
		else
			m_PB_Undo->setEnabled(true);
	}
	else if(Type==FVPanelGUI::Undo)
	{
		if(m_Display->GetAlphasSize(FiberDisplay::Previous)<=1)
			m_PB_Undo->setEnabled(false);
		else
			m_PB_Undo->setEnabled(true);
	}
}

void FiberViewerLightGUI::OpenDistributionPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		if(m_LE_OutputFolder->text()=="")
		{
			QMessageBox::warning(this, "Warning", "No output folder specifed");
			BrowserOutput();
		}
		if(m_LE_OutputFolder->text()!="")
		{
			bool LoadSuccess=true;
			if(m_RB_Save->isChecked())
			{
				if(sender()==m_PB_Gravity)
				{
					m_DistributionGUI->SetMethod("Gravity");
					SaveDistanceTable("Gravity");
				}
				else if(sender()==m_PB_Hausdorff)
				{
					if(m_RB_DT->isChecked())
					{
						m_Display->UpdateDT();
						m_DistributionGUI->SetMethod("Hausdorff",true);
					}
					else
						m_DistributionGUI->SetMethod("Hausdorff");
					SaveDistanceTable("Hausdorff");
				}
				else if(sender()==m_PB_Mean)
				{
					if(m_RB_DT->isChecked())
					{
						m_Display->UpdateDT();
						m_DistributionGUI->SetMethod("Mean",true);
					}
					else
						m_DistributionGUI->SetMethod("Mean");
					SaveDistanceTable("Mean");
				}
			}
			else
			{
				if(sender()==m_PB_Gravity)
					LoadSuccess=LoadDistanceTable("Gravity");
				else if(sender()==m_PB_Hausdorff)
					LoadSuccess=LoadDistanceTable("Hausdorff");
				else if(sender()==m_PB_Mean)
					LoadSuccess=LoadDistanceTable("Mean");
				if(!LoadSuccess)
				{
					QMessageBox::warning(this, "Error", "Error loading distance table");
					return;
				}
			}
			m_GB_ActionPanel->hide();
			m_DistributionGUI->SetMaxDistance();
			m_GB_DistributionPanel->show();
			m_ProgressBar->setValue(0);
		}
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::OpenNormCutPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		if(m_LE_OutputFolder->text()=="")
		{
			QMessageBox::warning(this, "Warning", "No output folder specifed");
			BrowserOutput();
		}
		if(m_LE_OutputFolder->text()!="")
		{
			bool LoadSuccess=true;
			if(m_RB_Save->isChecked())
			{
				if(m_RB_DT->isChecked())
				{
					m_Display->UpdateDT();
					m_NormCutGUI->ApplyWeight(true);
				}
				else
					m_NormCutGUI->ApplyWeight();
				SaveDistanceTable("NormCut");
			}
			else
			{
				LoadSuccess=LoadDistanceTable("NormCut");
				if(!LoadSuccess)
				{
					QMessageBox::warning(this, "Error", "Error loading distance table");
					return;
				}
			}
			m_GB_ActionPanel->hide();
			m_GB_NormCutPanel->show();
			m_ProgressBar->setValue(0);
		}
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::OpenCutterPanel()
{
	if(m_LE_VTKInput->text()!="")
	{
		m_GB_ActionPanel->hide();
		m_GB_CutterPanel->show();
		if(!m_PlanSetting->isVisible())
			OpenPlanSetting();
	}
	else
		QMessageBox::warning(this, "Warning", "No Fiber selected!");
}

void FiberViewerLightGUI::CloseDistributionPanel(FVPanelGUI::ExitSignal Type)
{
	m_GB_DistributionPanel->hide();
	if(Type==FVPanelGUI::Undo)
	{
		m_DistributionGUI->ClearHist();
		m_GB_ActionPanel->show();
	}
	else if(Type==FVPanelGUI::Ok)
	{
		m_DisplayClassGUI->SetSender(FVDisplayClassGUI::Distribution);
		m_DisplayClassGUI->SetModel(m_DistributionGUI->GetMark(), m_DistributionGUI->GetNumberOfClasse());
		m_GB_DisplayClassPanel->show();
	}
}

void FiberViewerLightGUI::CloseDisplayClass(FVPanelGUI::ExitSignal Type)
{
	m_Display->UpdateCells();
	m_Display->SetLookupTable(m_RedMap);
	m_Display->Render();
	if(Type==FVPanelGUI::Undo)
	{
		m_GB_DisplayClassPanel->hide();
		if(m_DisplayClassGUI->GetSender()==FVDisplayClassGUI::Distribution)
			m_GB_DistributionPanel->show();
		else
			m_GB_NormCutPanel->show();
	}
	else if(Type==FVPanelGUI::Ok)
	{
		m_PB_Redo->setEnabled(false);
		if(m_DisplayClassGUI->GetSender()==FVDisplayClassGUI::Distribution)
			m_DistributionGUI->ClearHist();
		m_GB_DisplayClassPanel->hide();
		m_GB_ActionPanel->show();
		if(m_Display->AlphasIsUnchanged())
		{
			if(m_Display->GetAlphasSize(FiberDisplay::Previous)>1)
				m_PB_Undo->setEnabled(true);
			else
				m_PB_Undo->setEnabled(false);
		}
		else
			m_PB_Undo->setEnabled(true);
	}
}

void FiberViewerLightGUI::CloseNormCut(FVPanelGUI::ExitSignal Type)
{
	m_Display->UpdateCells();
	m_Display->SetLookupTable(m_RedMap);
	m_Display->Render();
	m_GB_NormCutPanel->hide();
	if(Type==FVPanelGUI::Ok)
	{
		m_DisplayClassGUI->SetSender(FVDisplayClassGUI::NCut);
		m_DisplayClassGUI->SetModel(m_NormCutGUI->GetMark(), m_NormCutGUI->GetNumberOfClasse());
		m_GB_DisplayClassPanel->show();
	}
	else if(Type==FVPanelGUI::Undo)
		m_GB_ActionPanel->show();
}

void FiberViewerLightGUI::CloseCutter(FVPanelGUI::ExitSignal Type)
{
	m_GB_CutterPanel->hide();
	m_GB_ActionPanel->show();
	if(m_PlanSetting->isVisible())
		OpenPlanSetting();
	if(Type==FVPanelGUI::Ok)
		m_PB_Undo->setEnabled(false);
}

void FiberViewerLightGUI::OpenPlanSetting()
{
	if(m_PlanSetting->isVisible())
	{
		m_Display->GetPlan()->Off();
		m_PlanSetting->hide();
	}
	else
	{
		m_Display->GetPlan()->On();
      //Check if there is actually a plan displayed
		if(m_PlanSetting)
		{
			m_PlanSetting->GetParam();
			m_PlanSetting->show();
		}
	}
}

void FiberViewerLightGUI::UndoAction()
{
	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Previous),FiberDisplay::Next);
	m_Display->PopBackAlpha(FiberDisplay::Previous);
	m_Display->UpdateCells();
	m_Display->Render();
	m_PB_Redo->setEnabled(true);
	if(m_Display->GetAlphasSize(FiberDisplay::Previous)<=1)
		m_PB_Undo->setEnabled(false);
}

void FiberViewerLightGUI::RedoAction()
{
	m_Display->PushBackAlpha(m_Display->GetLastAlpha(FiberDisplay::Next),FiberDisplay::Previous);
	m_Display->PopBackAlpha(FiberDisplay::Next);
	m_Display->UpdateCells();
	m_Display->Render();
	m_PB_Undo->setEnabled(true);
	if(m_Display->GetAlphasSize(FiberDisplay::Next)<=0)
		m_PB_Redo->setEnabled(false);
}

void FiberViewerLightGUI::SaveDistanceTable(std::string Process)
{
	std::string Fibername=m_VtkFileName;
	Fibername=Fibername.substr(Fibername.find_last_of("/")+1,Fibername.size()-Fibername.find_last_of("/"));
	Fibername=Fibername.substr(0,Fibername.find_first_of("."));
	std::string Filename;
	if(*(m_OutputFolder.end()-1)=='/')
		Filename=m_OutputFolder+Fibername+"_"+Process+".txt";
	else
		Filename=m_OutputFolder+"/"+Fibername+"_"+Process+".txt";
	std::ofstream SaveFile(Filename.c_str());
	if(SaveFile)
	{
		std::vector<std::vector<double> > Distance;
		if(Process=="NormCut")
			Distance=m_NormCutGUI->GetDistanceTable();
		else
			Distance=m_DistributionGUI->GetDistanceTable();
		
		SaveFile<<"Process : "<<Process<<std::endl;
		for(int i=0; i<Distance.size(); i++)
		{
			for(int j=0; j<Distance[i].size(); j++)
				SaveFile<<Distance[i][j]<<" ";
			SaveFile<<std::endl;
		}
		SaveFile.close();
	}
	else
		std::cout<<"Error: Cannot open save file."<<std::endl;
}

bool FiberViewerLightGUI::LoadDistanceTable(std::string Process)
{
	std::string Fibername=m_LE_VTKInput->text().toStdString();
	Fibername=Fibername.substr(Fibername.find_last_of("/")+1,Fibername.size()-Fibername.find_last_of("/"));
	Fibername=Fibername.substr(0,Fibername.find_first_of("."));
	std::string Filename;
	if(*(m_LE_OutputFolder->text().end()-1)=='/')
		Filename=m_LE_OutputFolder->text().toStdString()+Fibername+"_"+Process+".txt";
	else
		Filename=m_LE_OutputFolder->text().toStdString()+"/"+Fibername+"_"+Process+".txt";
	std::ifstream File(Filename.c_str(), std::ios::in);
	std::string Buffer;
	if(File)
	{
		getline(File,Buffer);
		if(Buffer.compare(0, 10,"Process : ")==0)
		{
			std::vector<std::vector<double> > Distance;
			while(!File.eof())
			{
				getline(File,Buffer);
				std::vector<double> Line;
				std::istringstream iss(Buffer);
				double Value;
				while(!iss.eof())
				{
					iss>>Value;
					Line.push_back(Value);
				}
				Distance.push_back(Line);
			}
			if(Process=="NormCut")
				m_NormCutGUI->SetDistanceTable(Distance);
			else
				m_DistributionGUI->SetDistanceTable(Distance);
		}
		else
		{
			QString qs=Process.c_str();
			qs="Wrong syntax for "+qs+" distance table";
			QMessageBox::information(this, "Error", qs);
			return false;
		}
	}
	else
	{
		QString qs=Process.c_str();
		qs="Couldn't find distance table for "+qs;
		QMessageBox::information(this, "Error", qs);
		return false;
	}
	return true;
}

bool FiberViewerLightGUI::ProcessWithoutGUI(std::string Input, std::string OutputFolder, std::vector<std::string> ProcessList, bool DT, int Voxels)
{
	double Spacing, Bounds[6];
	m_VtkFileName=Input;
	m_OutputFolder=OutputFolder;
	if(m_VtkFileName.find_last_of("/")==std::string::npos)
		m_VtkFileName="./"+m_VtkFileName;
	vtkSmartPointer<vtkPolyData> PolyData=LoadVTK(m_VtkFileName);
	m_Display->SetOriginalPolyData(PolyData);
	m_Display->GetBounds(Bounds);
	if(Voxels==-1)
		Spacing=1;
	else if(Voxels>0)
		Spacing=ceil(Bounds[1]-Bounds[0]+1)/Voxels;
	else
	{
		std::cout<<"Wrong number of Voxels specified"<<std::endl;
		return false;
	}
	m_Display->SetSpacing(Spacing);
	for(int i=0; i<ProcessList.size(); i++)
	{
		if(ProcessList[i]=="Gravity")
		{
			std::cout<<"Computing Gravity method..."<<std::endl;
			m_DistributionGUI->SetMethod("Gravity");
			SaveDistanceTable("Gravity");
			std::cout<<"End of computation for Gravity method."<<std::endl;
		}
		else if(ProcessList[i]=="Hausdorff")
		{
			std::cout<<"Computing Hausdorff method..."<<std::endl;
			if(DT)
			{
				m_Display->UpdateDT();
				m_DistributionGUI->SetMethod("Hausdorff",true);
			}
			else
				m_DistributionGUI->SetMethod("Hausdorff");
			SaveDistanceTable("Hausdorff");
			std::cout<<"End of computation for Hausdorff method."<<std::endl;
		}
		else if(ProcessList[i]=="Mean")
		{
			std::cout<<"Computing Mean method..."<<std::endl;
			if(DT)
			{
				m_Display->UpdateDT();
				m_DistributionGUI->SetMethod("Mean",true);
			}
			else
				m_DistributionGUI->SetMethod("Mean");
			SaveDistanceTable("Mean");
			std::cout<<"End of computation for Mean method."<<std::endl;
		}
		else if(ProcessList[i]=="NormCut")
		{
			std::cout<<"Computing NormCut method..."<<std::endl;
			if(DT)
			{
				m_Display->UpdateDT();
				m_NormCutGUI->ApplyWeight(true);
			}
			else
				m_NormCutGUI->ApplyWeight();
			SaveDistanceTable("NormCut");
			std::cout<<"End of computation for NormCut method."<<std::endl;
		}
		else
			std::cout<<ProcessList[i]<<" is not an existing method."<<std::endl;
	}
}
