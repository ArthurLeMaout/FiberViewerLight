#include "FiberDisplay.h"
#include "FiberViewerLightGUI.h"


FiberDisplay::FiberDisplay(QWidget* parent) : QWidget(parent)
{
	m_OriginalPolyData=vtkSmartPointer<vtkPolyData>::New();
	m_DisplayedPolyData=vtkSmartPointer<vtkPolyData>::New();
	iren=vtkSmartPointer<QVTKInteractor>::New();
	m_Plane=vtkSmartPointer<vtkImplicitPlaneWidget>::New();
	m_VTKW_RenderWin=new QVTKWidget;
	m_VTKW_RenderWin->setMinimumSize(580,744);
	
	QGridLayout* MainLayout=new QGridLayout;
	MainLayout->addWidget(m_VTKW_RenderWin);
	setLayout(MainLayout);
	
	InitRenderer();
}

/********************************************************************************
 *InitRenderer: Create a new renderer and delete previous ones
 ********************************************************************************/

void FiberDisplay::InitRenderer()
{
	vtkSmartPointer<vtkCamera> camera=vtkSmartPointer<vtkCamera>::New();
	camera->SetPosition(1,1,1);
	camera->SetFocalPoint(0,0,0);
	
	//Delete the renderer if there is already one
	if(m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetNumberOfItems()>0)
	{
		vtkRenderer* renderer=m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		m_VTKW_RenderWin->GetRenderWindow()->RemoveRenderer(renderer);
	}
	
	//Create a new renderer
	vtkSmartPointer<vtkRenderer> renderer=vtkSmartPointer<vtkRenderer>::New();
	renderer->SetActiveCamera(camera);
	renderer->ResetCamera();
	renderer->SetBackground(0.6,0.6,0.6);
	
	m_VTKW_RenderWin->GetRenderWindow()->AddRenderer(renderer);
	iren->SetRenderWindow(m_VTKW_RenderWin->GetRenderWindow());
	vtkSmartPointer<vtkInteractorStyleSwitch> style=vtkSmartPointer<vtkInteractorStyleSwitch>::New();
	style->SetCurrentStyleToTrackballCamera();
	iren->SetInteractorStyle(style);
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

void FiberDisplay::InitAlphas()
{
	m_PreviousAlphas.push_back(std::vector<int>());
	for(int i=0; i<m_OriginalPolyData->GetNumberOfCells(); i++)
		m_PreviousAlphas[0].push_back(1);
}

void FiberDisplay::InitDTVector()
{
	m_DTVector.clear();
	int MaxFibers=m_OriginalPolyData->GetNumberOfCells();
	for(int i=0; i<MaxFibers; i++)
		m_DTVector.push_back(NULL);
}

bool FiberDisplay::IsUnchanged()
{
	if(m_PreviousAlphas.size()>1)
	{
		for(int i=0; i<m_OriginalPolyData->GetNumberOfCells(); i++)
		{
			if(m_PreviousAlphas[m_PreviousAlphas.size()-1][i]!=m_PreviousAlphas[m_PreviousAlphas.size()-2][i])
				return false;
		}
	}
	PopBackAlpha(FiberDisplay::Previous);
	return true;
}

vtkSmartPointer<vtkPolyData> FiberDisplay::GetOriginalPolyData()
{
	return m_OriginalPolyData;
}

vtkSmartPointer<vtkRenderer> FiberDisplay::GetRenderer()
{
		return m_VTKW_RenderWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
}

vtkSmartPointer<vtkActor> FiberDisplay::GetActor()
{
	vtkActorCollection* Collection=GetRenderer()->GetActors();
	Collection->InitTraversal();
	return Collection->GetNextActor();
}

std::vector<int> FiberDisplay::GetLastAlpha(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		return m_NextAlphas[m_NextAlphas.size()-1];
	else if(Type==FiberDisplay::Previous)
		return m_PreviousAlphas[m_PreviousAlphas.size()-1];
	else
		return std::vector<int>();
}

int FiberDisplay::GetAlphasSize(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		return m_NextAlphas.size();
	else if(Type==FiberDisplay::Previous)
		return m_PreviousAlphas.size();
	else
		return 0;
}
/********************************************************************************
 *GetFiberColor: Color Mapping, coef is a value between 0 and 1 and color is 
 *	the RGB results also between 0 and 1. Doing a linear interpolation between
 *	purple and red.
 ********************************************************************************/

void FiberDisplay::GetFiberColor(double coef, double color[])
{
	coef=(1-coef)*1020.0;
	double contrast=0.9;
	
	if(coef<256)
	{
		color[0]=(int)(255*contrast); 
		color[1]=(int)(int(coef)*contrast); 
		color[2]=0;
	}
	else if(coef>=256 && coef<510)
	{
		color[0]=(int)((255-int(coef)%255)*contrast);
		color[1]=(int)(255*contrast);
		color[2]=0;
	}
	else if(coef>=510 && coef<764)
	{
		color[0]=0;
		color[1]=(int)(255*contrast);
		color[2]=(int)((int(coef)%255+1)*contrast);
	}
	else if(coef>=764 && coef<1021)
	{
		color[0]=0;
		color[1]=(int)((255-int(coef)%255)*contrast);
		color[2]=(int)(255*contrast);
	}
	for(int i=0; i<3; i++)
		color[i]/=255.0;
}

void FiberDisplay::GetSpacing(double Spacing[])
{
	Spacing[0]=m_Spacing[0];
	Spacing[1]=m_Spacing[1];
	Spacing[2]=m_Spacing[2];
}

RealImageType::Pointer FiberDisplay::GetDTVector(int Id)
{
	return m_DTVector[Id];
}

void FiberDisplay::GetBounds(double Bounds[])
{
	Bounds[0]=m_Bounds[0];
	Bounds[1]=m_Bounds[1];
	Bounds[2]=m_Bounds[2];
	Bounds[3]=m_Bounds[3];
	Bounds[4]=m_Bounds[4];
	Bounds[5]=m_Bounds[5];
}

void FiberDisplay::SetSpacing(double Spacing[])
{
	if(Spacing[0]!=m_Spacing[0] || Spacing[1]!=m_Spacing[1] || Spacing[2]!=m_Spacing[2])
	{
		InitDTVector();
		m_Spacing[0]=Spacing[0];
		m_Spacing[1]=Spacing[1];
		m_Spacing[2]=Spacing[2];
	}
}

void FiberDisplay::SetOriginalPolyData(vtkSmartPointer<vtkPolyData> PolyData)
{
	m_OriginalPolyData->DeepCopy(PolyData);
	m_OriginalPolyData->BuildCells();
	InitAlphas();
	InitBounds();
	m_Spacing[0]=1;
	m_Spacing[1]=1;
	m_Spacing[2]=1;
	InitDTVector();
}

	//Set Mapper's LUT Color Map
void FiberDisplay::SetLookupTable(vtkSmartPointer<vtkLookupTable> Map)
{
	GetActor()->GetMapper()->SetLookupTable(Map);
}

void FiberDisplay::SetLastAlpha(std::vector<int> Alpha, AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas[m_NextAlphas.size()-1]=Alpha;
	else if(Type==FiberDisplay::Previous)
	{
		m_PreviousAlphas[m_PreviousAlphas.size()-1]=Alpha;
		emit(NbFibersChanged(GetNbModifiedFibers()));
	}
}

void FiberDisplay::SetNbFibersDisplayed(int value)
{
	m_NbFibersDisplayed=value;
}

void FiberDisplay::PushBackAlpha(std::vector<int> Alpha, AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas.push_back(Alpha);
	else if(Type==FiberDisplay::Previous)
	{
		m_PreviousAlphas.push_back(Alpha);
		emit(NbFibersChanged(GetNbModifiedFibers()));
	}
}

void FiberDisplay::PopBackAlpha(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas.pop_back();
	else if(m_PreviousAlphas.size()>1 && Type==FiberDisplay::Previous)
	{
		m_PreviousAlphas.pop_back();
		emit(NbFibersChanged(GetNbModifiedFibers()));
	}
}

void FiberDisplay::ClearAlphas(AlphasType Type)
{
	if(Type==FiberDisplay::Next)
		m_NextAlphas.clear();
	else if(Type==FiberDisplay::Previous)
		m_PreviousAlphas.clear();
}

void FiberDisplay::InitBounds()
{
	vtkPoints* Points=m_OriginalPolyData->GetPoints();
	double MinY=10000, MaxY=-10000, MinZ=10000, MaxZ=-10000, MinX=10000, MaxX=-10000;
	for(int i=0; i<m_OriginalPolyData->GetNumberOfPoints(); i++)
	{
		double Point[3];
		Points->GetPoint(i,Point);
		if(Point[0]<MinX)
			MinX=Point[0];
		if(Point[0]>MaxX)
			MaxX=Point[0];
		if(Point[1]<MinY)
			MinY=Point[1];
		if(Point[1]>MaxY)
			MaxY=Point[1];
		if(Point[2]<MinZ)
			MinZ=Point[2];
		if(Point[2]>MaxZ)
			MaxZ=Point[2];
	}
	m_Bounds[0]=MinX;
	m_Bounds[1]=MaxX;
	m_Bounds[2]=MinY;
	m_Bounds[3]=MaxY;
	m_Bounds[4]=MinZ;
	m_Bounds[5]=MaxZ;
}

void FiberDisplay::InitPlan()
{
	m_Plane->SetPlaceFactor(1);
	m_Plane->PlaceWidget(m_Bounds);
	m_Plane->SetInteractor(iren);
	m_Plane->SetOrigin((m_Bounds[0]+m_Bounds[1])/2,(m_Bounds[2]+m_Bounds[3])/2,(m_Bounds[4]+m_Bounds[5])/2);
	m_Plane->SetNormal(1,0,0);
	m_Plane->UpdatePlacement();
	m_Plane->Off();
}


int FiberDisplay::GetNbModifiedFibers()
{
	int NbModifiedFibers=0;
	std::vector<int> Alpha=GetLastAlpha(FiberDisplay::Previous);
	for(int i=0; i<Alpha.size(); i++)
	{
		if(Alpha[i]==1)
			NbModifiedFibers++;
	}
	return NbModifiedFibers;
}

std::vector<int> FiberDisplay::GenerateRandomIds()
{
	srand(time(NULL));
	std::vector<int> RandomIds;
	int NumberOfFibers=GetNbModifiedFibers();
	int Random;
	int NbOfRandomIds;
	RandomIds.push_back(Random);
	if( ((double)m_NbFibersDisplayed/(double)NumberOfFibers)*100 < 50 )
		NbOfRandomIds=m_NbFibersDisplayed;
	else
		NbOfRandomIds=NumberOfFibers-m_NbFibersDisplayed;
	if(NbOfRandomIds!=0)
	{
		Random=rand()%NumberOfFibers;
		RandomIds.push_back(Random);
	}
	for(int i=1; i<NbOfRandomIds; i++)
	{
		Random=rand()%NumberOfFibers;
		while(std::find(RandomIds.begin(), RandomIds.end(), Random)!=RandomIds.end())
			Random++;
		RandomIds.push_back(Random);
	}
	sort(RandomIds.begin(), RandomIds.end());
	return RandomIds;	
}

void FiberDisplay::FillDisplayedId(std::vector<int> RandomIds)
{
	m_DisplayedId.clear();
	int RandCountId=0, RandCount=0;
	std::vector <int> Alpha=GetLastAlpha(FiberDisplay::Previous);
	int NumberOfFibers=GetNbModifiedFibers();
	for(int i=0; i<Alpha.size(); i++)
	{
		if(Alpha[i]==0)
			m_DisplayedId.push_back(0);
		else if(Alpha[i]==1)
		{
			if( ((double)m_NbFibersDisplayed/(double)NumberOfFibers)*100<50)
			{
				if(RandomIds[RandCountId]==RandCount)
				{
					RandCountId++;
					m_DisplayedId.push_back(1);
				}
				else
					m_DisplayedId.push_back(0);
			}
			else
			{
				if(RandomIds[RandCountId]==RandCount)
				{
					RandCountId++;
					m_DisplayedId.push_back(0);
				}
				else
					m_DisplayedId.push_back(1);
			}
			RandCount++;
		}
	}
}

void FiberDisplay::StartRenderer(vtkSmartPointer<vtkPolyData> PolyData)
{
	//Copy PolyData in Original in case of step back
	m_OriginalPolyData->DeepCopy(PolyData);
	m_OriginalPolyData->BuildCells();
	m_DisplayedPolyData=PolyData;
	InitAlphas();
	
	//Get actual renderer
	vtkSmartPointer<vtkRenderer> Renderer=vtkSmartPointer<vtkRenderer>::New();
	Renderer=GetRenderer();
	if(Renderer!=NULL)
	{
		InitBounds();
		InitPlan();
		m_Spacing[0]=1;
		m_Spacing[1]=1;
		m_Spacing[2]=1;
		
		//Set mapper's input
		vtkSmartPointer<vtkPolyDataMapper> PolyDataMapper=vtkSmartPointer<vtkPolyDataMapper>::New();
		PolyDataMapper->SetInput(m_DisplayedPolyData);
		
		//Set actor's mapper
		vtkSmartPointer<vtkActor> PolyDataActor=vtkSmartPointer<vtkActor>::New();
		PolyDataActor->SetMapper(PolyDataMapper);
		
		//Add actor and set actor's color
		Renderer->AddActor(PolyDataActor);	
		Renderer->ResetCamera();
		Render();
	}
	else
		std::cout<<"No Renderer in the Render Window."<<std::endl;
}

void FiberDisplay::Render()
{
	m_VTKW_RenderWin->GetRenderWindow()->Render();
}

void FiberDisplay::UpdateCells()
{
	FillDisplayedId(GenerateRandomIds());
	vtkSmartPointer<vtkPolyData> ModifiedPolyData=vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkFloatArray> NewScalars=vtkSmartPointer<vtkFloatArray>::New();
	vtkSmartPointer<vtkPoints> NewPoints=vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> NewLines=vtkSmartPointer<vtkCellArray>::New();
	vtkDataArray* Scalars=m_OriginalPolyData->GetPointData()->GetScalars();
	
	vtkPoints* Points=m_OriginalPolyData->GetPoints();
	vtkCellArray* Lines=m_OriginalPolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	int NewId=0;
	Lines->InitTraversal();
	for(int i=0; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		if(m_DisplayedId[i]==1)
		{
			vtkSmartPointer<vtkPolyLine> NewLine=vtkSmartPointer<vtkPolyLine>::New();
			NewLine->GetPointIds()->SetNumberOfIds(NumberOfPoints);
			for(int j=0; j<NumberOfPoints; j++)
			{
				NewPoints->InsertNextPoint(Points->GetPoint(Ids[j]));
				NewLine->GetPointIds()->SetId(j,NewId);
				NewId++;
				NewScalars->InsertNextValue(Scalars->GetComponent(0,Ids[j]));
			}
			NewLines->InsertNextCell(NewLine);
		}
		
	}
	ModifiedPolyData->SetPoints(NewPoints);
	ModifiedPolyData->SetLines(NewLines);
	ModifiedPolyData->GetPointData()->SetScalars(NewScalars);
	m_DisplayedPolyData->DeepCopy(ModifiedPolyData);
	Render();
}

IntImageType::Pointer FiberDisplay::Voxelize(int Id)
{
	IntImageType::Pointer LabelImage=IntImageType::New();
	double Origin[3];
	LabelImage->SetSpacing(m_Spacing);
	
	Origin[0]=m_Bounds[0];
	Origin[1]=m_Bounds[2];
	Origin[2]=m_Bounds[4];
	LabelImage->SetOrigin(Origin);
	//LabelImage->SetDirection(tensorreader->GetOutput()->GetDirection());
	IntImageType::RegionType Region;
	IntImageType::SizeType RegionSize;
	RegionSize[0]=ceil(m_Bounds[1]-m_Bounds[0]+1)/m_Spacing[0];
	RegionSize[1]=ceil(m_Bounds[3]-m_Bounds[2]+1)/m_Spacing[1];
	RegionSize[2]=ceil(m_Bounds[5]-m_Bounds[4]+1)/m_Spacing[2];
	
	IntImageType::IndexType RegionIndex;
	RegionIndex[0]=0;
	RegionIndex[1]=0;
	RegionIndex[2]=0;
	
	Region.SetSize(RegionSize);
	Region.SetIndex(RegionIndex);
	LabelImage->SetRegions(Region);
	
	LabelImage->Allocate();
	LabelImage->FillBuffer(0);
	
	vtkIdType NbPoints;
	vtkIdType* PointIds;
	vtkPoints* Points=m_OriginalPolyData->GetPoints();
	m_OriginalPolyData->GetCellPoints(Id, NbPoints, PointIds);
	
	for(int i=0; i<NbPoints; i++)
	{
		double Point[3];
		Points->GetPoint(PointIds[i], Point);
		itk::Point<double,3> ITKPoint;
		ITKPoint[0]=Point[0];
		ITKPoint[1]=Point[1];
		ITKPoint[2]=Point[2];
		
		ContinuousIndexType ContId;
		itk::Index<3> Index;
		LabelImage->TransformPhysicalPointToContinuousIndex(ITKPoint, ContId);
		Index[0]=static_cast<long int>(vnl_math_rnd_halfinttoeven(ContId[0]));
		Index[1]=static_cast<long int>(vnl_math_rnd_halfinttoeven(ContId[1]));
		Index[2]=static_cast<long int>(vnl_math_rnd_halfinttoeven(ContId[2]));
		if(!LabelImage->GetLargestPossibleRegion().IsInside(Index))
		{
			std::cerr << "Error index: " << Index << " not in image"  << std::endl;
			std::cout << "Ignoring" << std::endl;
			return NULL;
		}
		else
		{
			LabelImage->SetPixel(Index, 1);
		}
	}
	return LabelImage;
}
	

void FiberDisplay::UpdateDT()
{
	int MaxFibers=m_OriginalPolyData->GetNumberOfCells();
	std::vector<int> Alpha=GetLastAlpha(FiberDisplay::Previous);
	typedef itk::DanielssonDistanceMapImageFilter <IntImageType,RealImageType> DanielssonType;
	
	for(int i=0; i<MaxFibers; i++)
	{
		if(Alpha[i]==1 && !m_DTVector[i])
		{
			DanielssonType::Pointer DanielssonFilter=DanielssonType::New();
			IntImageType::Pointer LabelImage=IntImageType::New();
			LabelImage=Voxelize(i);
			m_DTVector[i]=RealImageType::New();
			DanielssonFilter->SetInput(LabelImage);
			DanielssonFilter->Update();
			m_DTVector[i]=DanielssonFilter->GetDistanceMap();
		}
		emit Progress((double)i*100.0/(double)MaxFibers);
	}
}

vtkSmartPointer<vtkImplicitPlaneWidget> FiberDisplay::GetPlan()
{
	return m_Plane;
}

