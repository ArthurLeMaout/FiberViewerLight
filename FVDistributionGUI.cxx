#include "FVDistributionGUI.h"

FVDistributionGUI::FVDistributionGUI(QWidget* Parent, FiberDisplay* FVDisplay) : FVPanelGUI(Parent,FVDisplay)
{
	m_HistPlot=new QwtPlot;
	m_HistPlot->setMinimumSize(300,300);
	m_HistPlot->setAxisTitle(QwtPlot::yLeft,"Frequency");
	m_HistPlot->setAxisTitle(QwtPlot::xBottom,"Distance Threshold (mm)");
	m_Hist=new QwtPlotHistogram;
	m_Hist->setPen(QPen(Qt::white,0));
	m_Hist->setBrush(QColor(51,0,153));
	m_Hist->attach(m_HistPlot);
	QwtPlotGrid* Grid=new QwtPlotGrid;
	Grid->enableXMin(true);
	Grid->enableYMin(true);
	Grid->setMajPen(QPen(Qt::black,0,Qt::DotLine));
	Grid->setMinPen(QPen(Qt::white,0,Qt::DotLine));
	Grid->attach(m_HistPlot);
	m_HistPlot->setCanvasBackground(Qt::white);
	m_HistPlot->replot();

	m_L_Distribution=new QLabel("Computes classes distribution", this);
	m_L_ChooseThreshold=new QLabel("Choose threshold", this);
	m_L_Min=new QLabel("Min", this);
	m_L_NbBars=new QLabel("NbBars", this);
	m_L_Max=new QLabel("Max", this);
	m_L_Threshold=new QLabel("Threshold (mm)", this);
	m_LE_Min=new QLineEdit(this);
	m_LE_Min->setText("0");
	m_LE_NbBars=new QLineEdit(this);
	m_LE_NbBars->setText("10");
	m_LE_Max=new QLineEdit(this);

	m_LE_Threshold=new QLineEdit(this);
	m_LE_Threshold->setText("0.5");
	m_PB_ComputeDistribution=new QPushButton("Computes distribution",this);
	
	QFrame* F_HLine1=new QFrame;
	QFrame* F_HLine2=new QFrame;
	F_HLine1->setFrameShadow(QFrame::Plain);
	F_HLine1->setFrameShape(QFrame::HLine);
	F_HLine2->setFrameShadow(QFrame::Plain);
	F_HLine2->setFrameShape(QFrame::HLine);
	
	QGridLayout* GL_Distribution=new QGridLayout;
	QVBoxLayout* VL_PanelLayout=new QVBoxLayout;
	QHBoxLayout* HL_Threshold=new QHBoxLayout;
	QHBoxLayout* HL_UndoNext=new QHBoxLayout;
	
	VL_PanelLayout->addWidget(m_L_Distribution);
	VL_PanelLayout->addWidget(F_HLine1);
	GL_Distribution->addWidget(m_L_Min, 0, 0);
	GL_Distribution->addWidget(m_LE_Min, 0, 1);
	GL_Distribution->addWidget(m_L_NbBars, 0, 2);
	GL_Distribution->addWidget(m_LE_NbBars, 0, 3);
	GL_Distribution->addWidget(m_L_Max, 0, 4);
	GL_Distribution->addWidget(m_LE_Max, 0, 5);
	GL_Distribution->addWidget(m_PB_ComputeDistribution, 1, 0, 5, 0);
	VL_PanelLayout->addLayout(GL_Distribution);
	VL_PanelLayout->addWidget(m_HistPlot);
	VL_PanelLayout->addWidget(m_L_ChooseThreshold);
	VL_PanelLayout->addWidget(F_HLine2);
	HL_Threshold->addWidget(m_L_Threshold);
	HL_Threshold->addWidget(m_LE_Threshold);
	VL_PanelLayout->addLayout(HL_Threshold);
	HL_UndoNext->addWidget(m_PB_Undo);
	HL_UndoNext->addWidget(m_PB_Next);
	VL_PanelLayout->addLayout(HL_UndoNext);
	setLayout(VL_PanelLayout);
	
	connect(m_PB_Next, SIGNAL(clicked()), this, SLOT(NextAction()));
	connect(m_PB_ComputeDistribution, SIGNAL(clicked()), this, SLOT(Plot()));
	connect(m_PB_Undo, SIGNAL(clicked()), this, SLOT(UndoAction()));
}

void FVDistributionGUI::ClearHist()
{
	m_Hist->setSamples(QVector<QwtIntervalSample>());
	m_HistPlot->replot();
	m_Com.clear();
	m_Mark.clear();
	m_Distance.clear();
}

void FVDistributionGUI::InitDistance(int NbFibers)
{
	m_Distance.clear();
	for (int i = 0 ; i < NbFibers ; i++)
	{
		std::vector<double> Line;
		//for all the other fibers
		for (int j = 0 ; j < NbFibers ; j++)
			Line.push_back(0);
		m_Distance.push_back(Line);
	}
}

void FVDistributionGUI::SetMethod(std::string Sender, bool Type)
{
	m_Sender=Sender;
	int NbFibers=m_Display->GetNbModifiedFibers();
	InitDistance(NbFibers);
	if(m_Sender=="Gravity")
	{
		m_HistPlot->setTitle("Gravity");
		ApplyGravity(NbFibers);
	}
	else if(m_Sender=="Hausdorff")
	{
		m_HistPlot->setTitle("Hausdorff");
		ApplyHausdorffMean("Hausdorff", Type);
	}
	else if(m_Sender=="Mean")
	{
		m_HistPlot->setTitle("Mean");
		ApplyHausdorffMean("Mean", Type);
	}
}

void FVDistributionGUI::SetMaxDistance()
{
	int Max=(int)GetMaxDistance();
	std::ostringstream text;
	text<<Max;
	m_LE_Max->setText(text.str().c_str());
}

void FVDistributionGUI::ApplyGravity(int NbFibers)
{
	double CountProgress=0;
	ComputeCOM();
	for (int i = 0 ; i < NbFibers ; i++)
	{
		//for all the other fibers
		for (int j = i ; j < NbFibers ; j++)
		{
			m_Distance[i][j] = ComputeGravDist(i,j);
			m_Distance[j][i] = m_Distance[i][j];
			CountProgress++;
			emit Progress(CountProgress*200.0/(double)(NbFibers*NbFibers));
		}
	}
}

void FVDistributionGUI::ApplyHausdorffMean(std::string Method, bool Type)
{
	int RelevantSourceFiberCount=0, RelevantTargetFiberCount=0;
	double CountProgress=0;
	
	std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkIdType NbSourcePoints, NbTargetPoints;
	vtkIdType* SourceIds;
	vtkIdType* TargetIds;
	int NbFibers=PolyData->GetNumberOfCells();
	
	//This loop will be done as many times as the number of fiber
	for (int i=0;i<NbFibers;i++)
	{
		if(Alpha[i]==1)
		{
			PolyData->GetCellPoints(i,NbSourcePoints,SourceIds);
			RelevantTargetFiberCount=RelevantSourceFiberCount+1;
			for (int j=i+1;j<NbFibers;j++)
			{
				if(Alpha[j]==1)
				{
					double Max, x1, x2;
					PolyData->GetCellPoints(j,NbTargetPoints,TargetIds);
					if(Method=="Hausdorff")
					{
						if(Type)
						{
							x1 = ComputeHausDist(j,NbSourcePoints,SourceIds);
							x2 = ComputeHausDist(i,NbTargetPoints,TargetIds);
						}
						else
						{
							x1 = ComputeHausDist(NbTargetPoints,TargetIds,NbSourcePoints,SourceIds);
							x2 = ComputeHausDist(NbSourcePoints,SourceIds,NbTargetPoints,TargetIds);
						}
						Max = x1;
						if (x2 > Max)
							Max = x2;
					}
					else if(Method=="Mean")
					{
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
						Max = (x1 + x2) / 2;
					}
					m_Distance[RelevantSourceFiberCount][RelevantTargetFiberCount] =  Max*m_Display->GetSpacing();
					m_Distance[RelevantTargetFiberCount][RelevantSourceFiberCount] =  Max*m_Display->GetSpacing();
					CountProgress++;
					RelevantTargetFiberCount++;
					emit Progress(CountProgress*200.0/(double)(NbFibers*NbFibers));
				}
			}
			RelevantSourceFiberCount++;
		}
	}
// 	for(size_t i=0; i<m_Distance.size(); i++)
// 	{
// 		for(size_t j=i+1; j<m_Distance.size(); j++)
// 			std::cout<<m_Distance[i][j]<<" ";
// 		std::cout<<std::endl;
// 	}
}

void FVDistributionGUI::Plot()
{
	QwtIntervalSeriesData* HistData=new QwtIntervalSeriesData;
	QVector<QwtIntervalSample> Samples;
	double Min=atof(m_LE_Min->text().toStdString().c_str());
	double Max=atof(m_LE_Max->text().toStdString().c_str());
	double Step=(Max-Min)/atof(m_LE_NbBars->text().toStdString().c_str());
	
	for(double i=Min; i+Step<=Max; i+=Step)
	{
		GenerateClasse(m_Distance, i);
		Samples.push_back(QwtIntervalSample(GetNumberOfClasse(), i, i+Step));
	}
	HistData->setSamples(Samples);
	m_Hist->setData(HistData);
	m_HistPlot->replot();
}

void FVDistributionGUI::GenerateClasse(std::vector<std::vector<double> > Distance, double Threshold)  
{	
	int NbFibers=m_Display->GetNbModifiedFibers();
	std::vector< std::vector< int> > Bond;
		std::vector<int> m_Norm;
	for(int i=0; i < NbFibers; i++)
		Bond.push_back(std::vector<int>());
	for(int i=1; i<NbFibers; i++)
	{
		for(int j=0; j<i; j++)
		{
			if(Distance[i][j]<=Threshold)
				Bond[i].push_back(j);
		}
	}
	
	m_Mark.clear();
	int numClasses = 0;
	for(int i=0;i<NbFibers; i++)
	{
		if(Bond[i].size()>0)
		{
			int currentClass = m_Mark[Bond[i][0]];
			m_Mark.push_back(currentClass);
			
			for(size_t j=1; j<Bond[i].size(); j++)
			{
				int queryClass = m_Mark[Bond[i][j]];
				if (queryClass != currentClass) 
				{
					
					for (size_t k=1; k<m_Mark.size(); k++)
					{
						if(m_Mark[k]==queryClass)
							m_Mark[k]=currentClass;
					}
				}
			}
		} else {
			m_Mark.push_back(numClasses);	
			numClasses++;
		}
	}
	
	  //Normalisation of the mark vector  
	for(int i = 0 ; i < NbFibers ; i++)
		m_Norm.push_back(-1);
	int Compt = 1;
	for(int i = 0 ; i < NbFibers ; i++)
	{
		if(m_Norm[m_Mark[i]] == -1)
			m_Norm[m_Mark[i]] = Compt++;
		m_Mark[i] = m_Norm[m_Mark[i]];
	}
}

double FVDistributionGUI::GetMaxDistance()
{
	double Max=-100000;
	for(size_t i=0; i<m_Distance.size(); i++)
	{
		for(size_t j=i; j<m_Distance[i].size(); j++)
		{
			if(m_Distance[i][j]>Max)
				Max=m_Distance[i][j];
		}
	}
	return Max;
}

int FVDistributionGUI::GetNumberOfClasse()
{
	//Get the max of the mark vector which is the number of classe
	int nbclasse = 0;
	for(size_t i = 0 ; i < m_Mark.size() ; i++)
		if(m_Mark[i] > nbclasse)
			nbclasse = m_Mark[i];
	return nbclasse;
}

void FVDistributionGUI::ComputeCOM()
{
	std::vector<int> Alpha=m_Display->GetLastAlpha(FiberDisplay::Previous);
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkPoints* Points=PolyData->GetPoints();
	vtkCellArray* Lines=PolyData->GetLines();
	vtkIdType* Ids;
	vtkIdType NumberOfPoints;
	
	double x,y,z;
	Lines->InitTraversal();
	for(int i = 0  ; Lines->GetNextCell(NumberOfPoints, Ids); i++)
	{
		if(Alpha[i]==1)
		{
			x = y = z = 0;
			//For each point of this fiber
			for(unsigned int j = 0 ; j < NumberOfPoints; j++)
			{
				double Point[3];
				Points->GetPoint(Ids[j], Point);
				x += Point[0];
				y += Point[1];
				z += Point[2];	
			}
			std::vector<double> com;
			com.push_back(x/NumberOfPoints);
			com.push_back(y/NumberOfPoints);
			com.push_back(z/NumberOfPoints);
			
			m_Com.push_back(com);
		}
	}
}

double FVDistributionGUI::ComputeGravDist(int source,int target)  
{
	double x = m_Com[target][0] - m_Com[source][0];
	double y = m_Com[target][1] - m_Com[source][1];
	double z = m_Com[target][2] - m_Com[source][2];
	double dist = sqrt(x*x + y*y + z*z);
	
	return dist;
}

double FVDistributionGUI::ComputeHausDist(int SourceId,int NbTargetPoints,vtkIdType* TargetIds)
{  
	double HausDist = -1;
	
	double Distance;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkPoints* Points=PolyData->GetPoints();
	RealImageType::Pointer DistanceMap=m_Display->GetDTVector(SourceId);
		
		for(int j=0; j<NbTargetPoints; j++)
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
			
			if (Distance > HausDist)
				HausDist = Distance;
		}
	return HausDist;
}

double FVDistributionGUI::ComputeHausDist(int NbSourcePoints,vtkIdType* SourceIds,int NbTargetPoints,vtkIdType* TargetIds)
{  
	double HausDist = -1;
	double x,y,z;
	double xs,ys,zs;
	double xt,yt,zt;
	
	double Distance, MinDist;
	
	vtkSmartPointer<vtkPolyData> PolyData;
	PolyData=m_Display->GetOriginalPolyData();
	vtkPoints* Points=PolyData->GetPoints();
// 	For each point of the source fiber
	for(int i=0; i<NbSourcePoints; i++)
	{
		double SourcePoint[3]={0,0,0};
		Points->GetPoint(SourceIds[i],SourcePoint);
		MinDist = 999999999;
// 		For each point of the target fiber
		xs=SourcePoint[0];
		ys=SourcePoint[1];
		zs=SourcePoint[2];
		
		for(int j=0; j<NbTargetPoints; j++)
		{
// 			calculate distance between the two points
			double TargetPoint[3]={0,0,0};
			Points->GetPoint(TargetIds[j],TargetPoint);
			
			xt = TargetPoint[0];
			yt = TargetPoint[1];
			zt = TargetPoint[2];
			x = (xs - xt);
			y = (ys - yt);
			z = (zs - zt);
			Distance = (x*x+y*y+z*z);
			
// 			Keep the minimum distance of the distances between the whole points
// 			of the target fiber and one point of the source fiber
			if (Distance<MinDist)
				MinDist = Distance;
		}
// 		 Finaly, get the maximum of all these minimum
		if (MinDist > HausDist)
			HausDist = MinDist;
	}
// 	return the MinMax, to have the Hausdorff distance
// 	it just remains to take the max between the couple i,j and j,i
		return sqrt(HausDist);
}


//Compute the MaxMin between two fibers
double FVDistributionGUI::ComputeMeanDistance(int SourceId,int NbTargetPoints,vtkIdType* TargetIds)
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

//Compute the MaxMin between two fibers
double FVDistributionGUI::ComputeMeanDistance(int NbSourcePoints,vtkIdType* SourceIds,int NbTargetPoints,vtkIdType* TargetIds)
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

void FVDistributionGUI::UndoAction()
{
	emit Exit(FVPanelGUI::Undo);
}

void FVDistributionGUI::NextAction()
{
	GenerateClasse(m_Distance, atof(m_LE_Threshold->text().toStdString().c_str()));
	emit Exit(FVPanelGUI::Ok);
}
