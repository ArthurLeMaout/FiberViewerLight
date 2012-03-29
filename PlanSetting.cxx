#include "PlanSetting.h"

PlanSetting::PlanSetting(QWidget* Parent, FiberDisplay* Display) : QWidget(Parent)
{
	m_Display=Display;
	m_PB_UpdatePlan=new QPushButton("Update", this);
	m_PB_UpdatePlan->setGeometry(23,70,127,20);
	m_PB_GetParam=new QPushButton("Get Param", this);
	m_PB_GetParam->setGeometry(155,70,127,20);
	m_PB_SavePlan=new QPushButton("Save Plan", this);
	m_PB_SavePlan->setGeometry(23,93,127,20);
	m_PB_LoadPlan=new QPushButton("Load Plan", this);
	m_PB_LoadPlan->setGeometry(155,93,127,20);
	m_L_X=new QLabel("X",this);
	m_L_X->setGeometry(108,0,16,17);
	m_L_Y=new QLabel("Y",this);
	m_L_Y->setGeometry(174,0,16,17);
	m_L_Z=new QLabel("Z",this);
	m_L_Z->setGeometry(243,0,16,17);
	m_L_Origin=new QLabel("Origin",this);
	m_L_Origin->setGeometry(25,20,38,21);
	m_L_Normal=new QLabel("Normal",this);
	m_L_Normal->setGeometry(25,45,45,21);
	m_LE_XOrigin=new QLineEdit(this);
	m_LE_XOrigin->setGeometry(75,20,66,23);
	m_LE_YOrigin=new QLineEdit(this);
	m_LE_YOrigin->setGeometry(144,20,66,23);
	m_LE_ZOrigin=new QLineEdit(this);
	m_LE_ZOrigin->setGeometry(213,20,66,23);
	m_LE_XNormal=new QLineEdit(this);
	m_LE_XNormal->setGeometry(75,45,66,23);
	m_LE_YNormal=new QLineEdit(this);
	m_LE_YNormal->setGeometry(144,45,66,23);
	m_LE_ZNormal=new QLineEdit(this);
	m_LE_ZNormal->setGeometry(213,45,66,23);
	setFixedSize(300,121);
	
	connect(m_PB_UpdatePlan, SIGNAL(clicked()), this, SLOT(UpdatePlan()));
	connect(m_PB_GetParam, SIGNAL(clicked()), this, SLOT(GetParam()));
	connect(m_PB_SavePlan, SIGNAL(clicked()), this, SLOT(SavePlan()));
	connect(m_PB_LoadPlan, SIGNAL(clicked()), this, SLOT(LoadPlan()));
}

void PlanSetting::UpdatePlan()
{
	m_Display->GetPlan()->SetNormal(m_LE_XNormal->text().toDouble(),m_LE_YNormal->text().toDouble(),m_LE_ZNormal->text().toDouble());
	m_Display->GetPlan()->SetOrigin(m_LE_XOrigin->text().toDouble(),m_LE_YOrigin->text().toDouble(),m_LE_ZOrigin->text().toDouble());
	m_Display->Render();
}

void PlanSetting::GetParam()
{
	double Param[3];
	std::ostringstream Value;
	
	m_Display->GetPlan()->GetOrigin(Param);
	Value<<Param[0];
	m_LE_XOrigin->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[1];
	m_LE_YOrigin->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[2];
	m_LE_ZOrigin->setText(Value.str().c_str());
	Value.str("");
	m_Display->GetPlan()->GetNormal(Param);
	Value<<Param[0];
	m_LE_XNormal->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[1];
	m_LE_YNormal->setText(Value.str().c_str());
	Value.str("");
	Value<<Param[2];
	m_LE_ZNormal->setText(Value.str().c_str());
}

void PlanSetting::SavePlan()
{
	GetParam();
	QString File = QFileDialog::getSaveFileName(this, "Save VTK file", ".", "FVP (*.fvp)");
	std::ofstream SaveFile(File.toStdString().c_str(), std::ios::out);
	if(SaveFile)
	{
		SaveFile<<"Cut Plane Origin: "<<m_LE_XOrigin->text().toDouble()<<" "<<m_LE_YOrigin->text().toDouble()<<" "<<m_LE_ZOrigin->text().toDouble()<<std::endl;;
		SaveFile<<"Cut Plane Normal: "<<m_LE_XNormal->text().toDouble()<<" "<<m_LE_YNormal->text().toDouble()<<" "<<m_LE_ZNormal->text().toDouble()<<std::endl;
	}
	else
		std::cout<<"ERROR: Problem to open the file for saving plane"<<std::endl;
}

void PlanSetting::LoadPlan()
{
	GetParam();
	QString Filename = QFileDialog::getOpenFileName(this, "Open FVP File", ".", "FVP (*.fvp)");
	
	std::ifstream File(Filename.toStdString().c_str(), std::ios::in);
	std::string Buffer;
	
	if(File)
	{
		std::ostringstream oss;
		float value;
		
		getline(File, Buffer);
		if(Buffer.compare(0,17,"Cut Plane Origin:")==0)
		{
			std::istringstream iss;
			iss.str(Buffer.substr(17,Buffer.size()-17));
			
			iss>>value;
			std::cout<<iss.str()<<" "<<value<<std::endl;
			oss<<value;
			m_LE_XOrigin->setText(oss.str().c_str());
			oss.str("");
			
			iss>>value;
			oss<<value;
			m_LE_YOrigin->setText(oss.str().c_str());
			oss.str("");
			
			iss>>value;
			oss<<value;
			m_LE_ZOrigin->setText(oss.str().c_str());
			oss.str("");
		}
		getline(File,Buffer);
		if(Buffer.compare(0,17,"Cut Plane Normal:")==0)
		{
			std::istringstream iss;
			iss.str(Buffer.substr(17,Buffer.size()-17));
			
			
			iss>>value;
			std::cout<<iss.str()<<" "<<value<<std::endl;
			oss<<value;
			m_LE_XNormal->setText(oss.str().c_str());
			oss.str("");
			
			iss>>value;
			oss<<value;
			m_LE_YNormal->setText(oss.str().c_str());
			oss.str("");
			
			iss>>value;
			oss<<value;
			m_LE_ZNormal->setText(oss.str().c_str());
			oss.str("");
		}
		UpdatePlan();
	}
	else
		std::cout<<"Error: Unable to open plane file."<<std::endl;
}

