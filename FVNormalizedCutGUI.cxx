#include "FVNormalizedCutGUI.h"

FVNormCutGUI::FVNormCutGUI(QWidget* Parent, FiberDisplay* FVDisplay):FVPanelGUI(Parent,FVDisplay)
{
	m_L_Cluster=new QLabel("Number of Cluster", this);
	m_SB_Cluster=new QSpinBox(this);
	m_SB_Cluster->setSingleStep(1);
	m_SB_Cluster->setRange(2,1000);
	m_SB_Cluster->setValue(2);
	
	
	QGridLayout* GL_NormCut=new QGridLayout;
	QHBoxLayout* HL_Navigation=new QHBoxLayout;
	
	GL_NormCut->addWidget(m_L_Cluster,0,0,1,0);
	GL_NormCut->addWidget(m_SB_Cluster,1,0,1,0);
	HL_Navigation->addWidget(m_PB_Undo);
	HL_Navigation->addWidget(m_PB_Next);
	GL_NormCut->addLayout(HL_Navigation,2,0,1,0);
	GL_NormCut->setRowStretch(3,1);

	setLayout(GL_NormCut);
	
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(NextAction()));	
}

void FVNormCutGUI::InitWeight()
{
	m_Weight.clear();
	int NbFibers=m_Display->GetNbModifiedFibers();
	for (int i = 0 ; i < NbFibers ; i++)
	{
		std::vector<double> Line;
		//for all the other fibers
		for (int j = 0 ; j < NbFibers ; j++)
			Line.push_back(0);
		m_Weight.push_back(Line);
	}
}

void FVNormCutGUI::ApplyWeight(bool Type)
{
	std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkIdType NbSourcePoints, NbTargetPoints;
	vtkIdType* SourceIds;
	vtkIdType* TargetIds;
	vtkCellArray* LinesSource=PolyData->GetLines();
	vtkCellArray* LinesTarget=PolyData->GetLines();
	int NbFibers=PolyData->GetNumberOfCells();
	int CountProgress=0;
	int RelevantSourceFiberCount=0, RelevantTargetFiberCount=0;
	InitWeight();
	for(int i=0; i<NbFibers; i++)
	{
		if(Alpha[i]==1)
		{
			PolyData->GetCellPoints(i,NbSourcePoints,SourceIds);
			RelevantTargetFiberCount=RelevantSourceFiberCount+1;
			for(int j=i+1; j<NbFibers; j++)
			{
				if(Alpha[j]==1)
				{
					PolyData->GetCellPoints(j,NbTargetPoints,TargetIds);
					double x1, x2;
					if(Type)
					{
						x1 = ComputeMeanDistance(j,NbSourcePoints,SourceIds);
						x2 = ComputeMeanDistance(i,NbTargetPoints,TargetIds);
					}
					else
					{
						x1 = ComputeMeanDistance(NbTargetPoints,TargetIds,NbSourcePoints,SourceIds);
						x2 = ComputeMeanDistance(NbSourcePoints,SourceIds,NbTargetPoints,TargetIds);
					}
					double MeanVal = (x1 + x2) / 2;
					if(MeanVal!=0)
					{
						m_Weight[RelevantSourceFiberCount][RelevantTargetFiberCount] = 1/(MeanVal*m_Display->GetSpacing());
						m_Weight[RelevantTargetFiberCount][RelevantSourceFiberCount] = 1/(MeanVal*m_Display->GetSpacing());
					}
					else
					{
						m_Weight[RelevantSourceFiberCount][RelevantTargetFiberCount]=0;
						m_Weight[RelevantTargetFiberCount][RelevantSourceFiberCount]=0;
					}
					CountProgress++;
					emit Progress(CountProgress*200/(NbFibers*NbFibers));
					RelevantTargetFiberCount++;
				}
			}
			RelevantSourceFiberCount++;
		}
	}
}

//Compute the MaxMin between two fibers
double FVNormCutGUI::ComputeMeanDistance(int NbSourcePoints,vtkIdType* SourceIds,int NbTargetPoints,vtkIdType* TargetIds)
{
	double x,y,z;
	double xs,ys,zs;
	double xt,yt,zt;
	
	double MeanDist, Distance, MinDist, TotalDist = 0;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkPoints* Points=PolyData->GetPoints();
	for (int i=0;i<NbSourcePoints;i++)
	{
		double SourcePoint[3]={0,0,0};
		Points->GetPoint(SourceIds[i],SourcePoint);
		MinDist = 999999999;
		//For each point of the target fiber
		xs=SourcePoint[0];
		ys=SourcePoint[1];
		zs=SourcePoint[2];
		
		for (int j=0;j<NbTargetPoints;j++)
		{
			//calculate distance between the two points
			double TargetPoint[3]={0,0,0};
			Points->GetPoint(TargetIds[j],TargetPoint);
			xt = TargetPoint[0];
			yt = TargetPoint[1];
			zt = TargetPoint[2];

			x = (xs - xt);
			y = (ys - yt);
			z = (zs - zt);
			Distance = (x*x+y*y+z*z);
			//Keep the minimum distance of the distances between the whole points
			//of the target fiber and one point of the source fiber
			if (Distance<MinDist)
				MinDist = Distance;
		}
		//Finaly, sum all min
		TotalDist += MinDist;
	}
	MeanDist = TotalDist/NbSourcePoints;
	//return the Meanmin to have the Mean distance
	//it just remains to take the mean between the couple i,j end j,i
	return sqrt(MeanDist);
}

//Compute the MaxMin between two fibers
double FVNormCutGUI::ComputeMeanDistance(int SourceId,int NbTargetPoints,vtkIdType* TargetIds)
{
	double MeanDist, Distance, TotalDist = 0;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkPoints* Points=PolyData->GetPoints();
	RealImageType::Pointer DistanceMap=m_Display->GetDTVector(SourceId);
		
	for (int j=0;j<NbTargetPoints;j++)
	{
		double TargetPoint[3]={0,0,0};
		Points->GetPoint(TargetIds[j],TargetPoint);
		itk::Point<double,3> ITKPoint;
		ITKPoint[0]=TargetPoint[0];
		ITKPoint[1]=TargetPoint[1];
		ITKPoint[2]=TargetPoint[2];
			
		ContinuousIndexType ContId;
		itk::Index<3> Index;
			
		DistanceMap->TransformPhysicalPointToContinuousIndex(ITKPoint, ContId);
		Index[0]=static_cast<long int>(vnl_math_rnd_halfinttoeven(ContId[0]));
		Index[1]=static_cast<long int>(vnl_math_rnd_halfinttoeven(ContId[1]));
		Index[2]=static_cast<long int>(vnl_math_rnd_halfinttoeven(ContId[2]));
		Distance=DistanceMap->GetPixel(Index);
		TotalDist += Distance;
	}
	MeanDist = TotalDist/NbTargetPoints;
	return MeanDist;
}

double FVNormCutGUI::Assoc(int BeginId, int EndId, bool All)
{
	double SumWeight=0;
	for(int i=BeginId; i<=EndId; i++)
	{
		if(!All)
		{
			for(int j=BeginId; j<=EndId; j++)
				SumWeight+=m_Weight[i][j];
		}
		else
		{
			for(unsigned int j=0; j<m_Weight.size() ; j++)
				SumWeight+=m_Weight[i][j];
		}
	}
	return SumWeight;
}

void FVNormCutGUI::GenerateClass()
{
	int ClusterDone=1;
	m_Mark=FillMark(m_Weight,ClusterDone,m_SB_Cluster->value());
}

std::vector<int> FVNormCutGUI::FillMark(std::vector<std::vector<double> > Weight, int& ClusterDone, int NumberOfCluster)
{
	std::vector<int> Mark;
	if(Weight.size()==1 || NumberOfCluster==1)
	{
		for(unsigned int i=0; i<Weight.size(); i++)
			Mark.push_back(ClusterDone);
		return Mark;
	}
	else
	{
		double MinCut=1000;
		int CutId=-1;
		for(unsigned int i=0; i<Weight.size()-1; i++)
		{
			double cut=Cut(i,i+1);
			double NCut=cut/Assoc(0,i,true)+cut/Assoc(i+1,Weight.size()-1,true);
			if(NCut<MinCut)
			{
				CutId=i;
				MinCut=NCut;
			}
		}
		std::vector<std::vector<double> > SubWeight;
		for(int i=0; i<=CutId; i++)
		{
			std::vector<double>Line;
			for(int j=0; j<=CutId; j++)
				Line.push_back(Weight[i][j]);
			SubWeight.push_back(Line);
		}
		Mark=FillMark(SubWeight,ClusterDone,NumberOfCluster/2);
		
		
		ClusterDone++;
		SubWeight.clear();
		for(unsigned int i=CutId+1; i<Weight.size(); i++)
		{
			std::vector<double>Line;
			for(unsigned int j=CutId+1; j<=Weight.size(); j++)
				Line.push_back(Weight[i][j]);
			SubWeight.push_back(Line);
		}
		std::vector<int> MarkTemp;
		
		if(NumberOfCluster%2==0)
			MarkTemp=FillMark(SubWeight,ClusterDone,NumberOfCluster/2);
		else
			MarkTemp=FillMark(SubWeight,ClusterDone,NumberOfCluster/2+1);
		for(unsigned int i=0; i<MarkTemp.size(); i++)
			Mark.push_back(MarkTemp[i]);
		return Mark;
	}
}

double FVNormCutGUI::Cut(int Previous, int Next)
{
	double Cut=0;
	for(int i=0; i<=Previous; i++)
	{
		for(unsigned int j=Next; j<m_Weight.size(); j++)
			Cut+=m_Weight[i][j];
	}
	return Cut;
}

int FVNormCutGUI::GetNumberOfClasse()
{
	//Get the max of the mark vector which is the number of classe
	int nbclasse = 0;
	for(unsigned int i = 0 ; i < m_Mark.size() ; i++)
		if(m_Mark[i] > nbclasse)
			nbclasse = m_Mark[i];
	return nbclasse;
}

void FVNormCutGUI::UndoAction()
{
	emit Exit(FVPanelGUI::Undo);
}

void FVNormCutGUI::NextAction()
{
	GenerateClass();
	emit Exit(FVPanelGUI::Ok);
}
