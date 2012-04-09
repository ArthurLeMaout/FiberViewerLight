#ifndef FVNORMCUTGUI_H
#define FVNORMCUTGUI_H

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkMath.h>

#include "FVPanelGUI.h"

class FVNormCutGUI: public FVPanelGUI
{
	Q_OBJECT
	
	public:
		FVNormCutGUI(QWidget* Parent, FiberDisplay* Display);
		void InitWeight();
		void ApplyWeight(bool Type=false);
		double ComputeMeanDistance(int NbSourcePoints,vtkIdType* SourceIds,int NbTargetPoints,vtkIdType* TargetIds);
		double ComputeMeanDistance(int SourceId,int NbTargetPoints,vtkIdType* TargetIds);
		std::vector<int> GetMark(){return m_Mark;}
		int GetNumberOfClasse();
		void GenerateClass();
		double Assoc(int BeginId, int EndId, bool All=false);
		double Cut(int Previous, int Next);
		std::vector<int> FillMark(std::vector<std::vector<double> > Weight, int& ClusterDone, int NumberOfCluster);
		std::vector<std::vector<double> > GetDistanceTable(){return m_Weight;}
		void SetDistanceTable(std::vector<std::vector<double> > Distance){m_Weight=Distance;}
		
	public slots:
		void UndoAction();
		void NextAction();
		
	private:
		std::vector<int> m_Mark;
		std::vector<std::vector<double> > m_Weight;
		QLabel* m_L_Cluster;
		QSpinBox* m_SB_Cluster;
};

#endif
