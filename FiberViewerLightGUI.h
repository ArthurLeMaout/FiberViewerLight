#ifndef FIBERVIEWERLIGHTGUI_H
#define FIBERVIEWERLIGHTGUI_H

#include <sstream>

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QApplication>
#include <QSpinBox>
#include <QMessageBox>
#include <QProgressBar>

#include "QVTKWidget.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
#include <vtkLookupTable.h>

#include "FiberDisplay.h"
#include "FVLengthGUI.h"
#include "FVDistributionGUI.h"
#include "FVNormalizedCutGUI.h"
#include "PlanSetting.h"

class FiberViewerLightGUI: public QWidget
{
	Q_OBJECT
			
	public:
		FiberViewerLightGUI(std::string input="", std::string output="", QWidget* parent=0);
		void InitWidgets();
		void InitRedMap(vtkPolyData* PolyData);
		vtkSmartPointer<vtkPolyData> LoadVTK(std::string FileName);
		void SetAtLastAlphas();
		int GetNbFiberDisplayed();
		
	protected slots:
		void BrowserVTKInput();
		void EnterVTKInput();
		void BrowserVTKOutput();
		void SaveVTK();
		void OpenLengthPanel();
		void OpenDistributionPanel();
		void OpenNormCutPanel();
		void CloseLengthPanel(FVPanelGUI::ExitSignal Direction);
		void CloseDistributionPanel(FVPanelGUI::ExitSignal Direction);
		void CloseDisplayClass(FVPanelGUI::ExitSignal Direction);
		void CloseNormCut(FVPanelGUI::ExitSignal Direction);
		void OpenPlanSetting();
		void UndoAction();
		void RedoAction();
		void UpdatePercentage(int value);
		void UpdateNbFibers(int value);
		void UpdateDisplayedLabel();
		void UpdateSpacing();
		
	private:
		QGroupBox* m_GB_ActionPanel;
		QGroupBox* m_GB_LengthPanel;
		QGroupBox* m_GB_DistributionPanel;
		QGroupBox* m_GB_DisplayClassPanel;
		QGroupBox* m_GB_NormCutPanel;
		QLineEdit* m_LE_VTKInput;
		QLineEdit* m_LE_VTKOutput;
		QLineEdit* m_LE_NbVoxelX;
		QLineEdit* m_LE_NbVoxelY;
		QLineEdit* m_LE_NbVoxelZ;
		QToolButton* m_TB_BrowserVTKInput;
		QToolButton* m_TB_BrowserVTKOutput;
		QLabel* m_L_VTKInput;
		QLabel* m_L_VTKOutput;
		QLabel* m_L_IO;
		QLabel* m_L_Processing;
		QLabel* m_L_NbVoxel;
		QLabel* m_L_NbVoxelX;
		QLabel* m_L_NbVoxelY;
		QLabel* m_L_NbVoxelZ;
		QLabel* m_L_Navigation;
		QPushButton* m_PB_Length;
		QPushButton* m_PB_Gravity;
		QPushButton* m_PB_Hausdorff;
		QPushButton* m_PB_Mean;
		QPushButton* m_PB_NormCut;
		QPushButton* m_PB_Undo;
		QPushButton* m_PB_Redo;
		QPushButton* m_PB_SaveVTK;
		QPushButton* m_PB_Plane;
		QProgressBar* m_ProgressBar;
		QSlider* m_S_PercentageDisplayed;
		QPushButton* m_PB_ApplyPercentage;
		QLabel* m_L_PercentageDisplayed;
		QLabel* m_L_NbFiber;
		QLabel* m_L_NbFiberDisplayed;
		std::string m_VtkFileName;
		
		FVLengthGUI* m_LengthGUI;
		FVDistributionGUI* m_DistributionGUI;
		FVDisplayClassGUI* m_DisplayClassGUI;
		FVNormCutGUI* m_NormCutGUI;
		vtkSmartPointer<vtkLookupTable> m_RedMap;
		FiberDisplay* m_Display;
		PlanSetting* m_PlanSetting;
};

#endif
