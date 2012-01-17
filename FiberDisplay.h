#ifndef FIBERDISPLAY_H
#define FIBERDISPLAY_H

#include <QWidget>
#include <QGridLayout>
#include "QVTKWidget.h"
#include <QMouseEvent>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPolyDataReader.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkImplicitPlaneWidget.h>

#include <time.h>
#include <algorithm>


class FiberDisplay: public QWidget
{
	Q_OBJECT
	
	public:
		enum AlphasType{Next,Previous};
		
		FiberDisplay(QWidget* parent);
		void InitRenderer();
		void InitAlphas();
		
		vtkPolyData* GetOriginalPolyData();
		vtkPolyData* GetModifiedPolyData();
		vtkRenderer* GetRenderer();
		vtkActor* GetActor();
		std::vector<int> GetLastAlpha(AlphasType Type);
		int GetAlphasSize(AlphasType Type);
		void GetFiberColor(double coef, double color[]);
		
		void SetOriginalPolyData(vtkPolyData* PolyData);
		void SetModifiedPolyData(vtkPolyData* PolyData);
		void SetLookupTable(vtkLookupTable* RedMap);
		void SetLastAlpha(std::vector<int> Alpha, AlphasType Type);
		
		void ClearAlphas(AlphasType Type);
		void PushBackAlpha(std::vector<int> Alpha, AlphasType Type);
		void PopBackAlpha(AlphasType Type);
		std::vector<int> GenerateRandomIds(vtkSmartPointer<vtkPolyData> PolyData);
		void StartRenderer(vtkPolyData* PolyData);
		void Render();
		void UpdateCells();
		void InitPlaneCoord(double Bounds[]);
		vtkImplicitPlaneWidget* GetPlan();
		void InitPlan(double Bounds[]);
		bool IsUnchanged();
		void UpdateDisplayedFibers();
		void SetNbFibersDisplayed(int value);
		
	signals:
		void NbFibersChanged(int);
	
	private:
		QVTKInteractor* iren;
		vtkImplicitPlaneWidget* m_Plane;
		QVTKWidget* m_VTKW_RenderWin;
		vtkSmartPointer<vtkPolyData> m_OriginalPolyData;
		vtkSmartPointer<vtkPolyData> m_ModifiedPolyData;
		vtkSmartPointer<vtkPolyData> m_DisplayedPolyData;
		std::vector<std::vector<int> > m_PreviousAlphas;
		std::vector<std::vector<int> > m_NextAlphas;
		int m_NbFibersDisplayed;
// 		std::vector<int> m_DisplayedId;
};

#endif
