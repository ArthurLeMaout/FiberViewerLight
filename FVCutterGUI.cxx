#include "FVCutterGUI.h"

FVCutterGUI::FVCutterGUI(QWidget* Parent, FiberDisplay* Display) : FVPanelGUI(Parent,Display)
{
	m_PB_Cut=new QPushButton("Cut",this);
	m_L_Detail=new QLabel("Remove fiber pieces on the arrowhead\nside of the plan",this);
	
	QGridLayout* GL_CutFilter=new QGridLayout;
	
	GL_CutFilter->addWidget(m_L_Detail,0,0,1,0);
	GL_CutFilter->addWidget(m_PB_Cut,1,0,1,0);
	GL_CutFilter->addWidget(m_PB_Undo,2,0);
	GL_CutFilter->addWidget(m_PB_Next,2,1);
	GL_CutFilter->setRowStretch(3,1);
	setLayout(GL_CutFilter);
	
	connect(m_PB_Cut, SIGNAL(clicked()), this, SLOT(Cut()));
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(OkAction()));
}

void FVCutterGUI::Cut()
{
	std::vector<int> LastPointsCut=m_Display->GetPointsCut();
	std::vector<int> PointsCut;
	vtkSmartPointer<vtkImplicitPlaneWidget> Plan=m_Display->GetPlan();
	double Normal[3], Origin[3];
	Plan->GetOrigin(Origin);
	Plan->GetNormal(Normal);
	
	vtkPoints* Points=m_Display->GetOriginalPolyData()->GetPoints();
	for(int i=0; i<Points->GetNumberOfPoints(); i++)
	{
		if(LastPointsCut[i]==1)
		{
			double Point[3],Vector[3],ScalarProduct;
			Points->GetPoint(i,Point);
			Vector[0]=Point[0]-Origin[0];
			Vector[1]=Point[1]-Origin[1];
			Vector[2]=Point[2]-Origin[2];
			ScalarProduct=Normal[0]*Vector[0]+Normal[1]*Vector[1]+Normal[2]*Vector[2];
			if(ScalarProduct>0)
				PointsCut.push_back(0);
			else
				PointsCut.push_back(1);
		}
		else
			PointsCut.push_back(0);
	}
	m_Display->SetPointsCut(PointsCut);
	m_Display->UpdateCells();
	m_Display->Render();
}

vtkSmartPointer<vtkPolyData> FVCutterGUI::BuildNewPolyData()
{
	std::vector<int> PointsCut=m_Display->GetPointsCut();
	std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	vtkSmartPointer<vtkPolyData> ModifiedPolyData=vtkSmartPointer<vtkPolyData>::New();
	
	vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
	vtkDataArray* Tensors=m_Display->GetOriginalPolyData()->GetPointData()->GetTensors();
	bool UseTensor=(Tensors!=NULL);
	vtkSmartPointer<vtkFloatArray> NewTensors=vtkSmartPointer<vtkFloatArray>::New();
	NewTensors->SetNumberOfComponents(9);
	vtkSmartPointer<vtkFloatArray> NewScalars=vtkSmartPointer<vtkFloatArray>::New();
	
	vtkDataArray* Scalars=m_Display->GetOriginalPolyData()->GetPointData()->GetScalars();
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
			std::vector<int> TempFiber;
			for(int j=0; j<NumberOfPoints; j++)
			{
				if(PointsCut[Ids[j]]==1)
				{
					TempFiber.push_back(NewId);
					NewPoints->InsertNextPoint(Points->GetPoint(Ids[j]));
					NewScalars->InsertNextValue(Scalars->GetComponent(0,Ids[j]));
					double tensorValue[9];
					if(UseTensor)
					{
						for(int k=0; k<9; k++)
							tensorValue[k]=Tensors->GetComponent(Ids[j],k);
						NewTensors->InsertNextTuple(tensorValue);
					}
					NewId++;
				}
				else if(TempFiber.size()>0)
				{
					vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New();
					NewLine->GetPointIds()->SetNumberOfIds(TempFiber.size());
					for(size_t k=0; k<TempFiber.size(); k++)
						NewLine->GetPointIds()->SetId(k,TempFiber[k]);
					NewLines->InsertNextCell(NewLine);
					TempFiber.clear();
				}
			}
			if(TempFiber.size()>0)
			{
				vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New();
				NewLine->GetPointIds()->SetNumberOfIds(TempFiber.size());
				for(size_t k=0; k<TempFiber.size(); k++)
					NewLine->GetPointIds()->SetId(k,TempFiber[k]);
				NewLines->InsertNextCell(NewLine);
			}
		}
	}
	ModifiedPolyData->GetPointData()->SetScalars(NewScalars);
	ModifiedPolyData->SetPoints(NewPoints);
	ModifiedPolyData->SetLines(NewLines);
	vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
	fiberwriter->SetFileName("./Test.vtk");
  #if (VTK_MAJOR_VERSION < 6)
	fiberwriter->SetInput(ModifiedPolyData);
  #else
	fiberwriter->SetInputData(ModifiedPolyData);
  #endif
	fiberwriter->Update();
	if(UseTensor)
		ModifiedPolyData->GetPointData()->SetTensors(NewTensors);
	return ModifiedPolyData;
}

/********************************************************************************
 *UndoAction: Undo last modification on the polydata.
 ********************************************************************************/

void FVCutterGUI::UndoAction()
{
	m_Display->InitPointsCut();
	m_Display->UpdateCells();
	m_Display->Render();
	emit Exit(FVPanelGUI::Undo);
}

void FVCutterGUI::OkAction()
{
	m_Display->SetOriginalPolyData(BuildNewPolyData());
	vtkSmartPointer<vtkPolyDataWriter> fiberwriter = vtkPolyDataWriter::New();
	fiberwriter->SetFileName("./Test2.vtk");
  #if (VTK_MAJOR_VERSION < 6)
	fiberwriter->SetInput(m_Display->GetOriginalPolyData());
  #else
	fiberwriter->SetInputData(m_Display->GetOriginalPolyData());
  #endif
	fiberwriter->Update();
	emit Exit(FVPanelGUI::Ok);
}
