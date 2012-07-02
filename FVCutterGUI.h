#ifndef FIBERCUTTER_H
#define FIBERCUTTER_H

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
#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_histogram.h"
#include "qwt_series_data.h"
#include "qwt_interval.h"

#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "FVPanelGUI.h"

class FVCutterGUI : public FVPanelGUI
{
	Q_OBJECT
	
	public:
		FVCutterGUI(QWidget* Parent, FiberDisplay* Display);
		vtkSmartPointer<vtkPolyData> BuildNewPolyData();
		
	protected slots:
		void Cut();
		void UndoAction();
		void OkAction();
		
	private:
		QLabel* m_L_Detail;
		QPushButton* m_PB_Cut;
};

#endif
