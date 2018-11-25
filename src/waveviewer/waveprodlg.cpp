#include <QtWidgets/QtWidgets>
#include "waveprodlg.h"

WaveProDlg::WaveProDlg(QWidget *parent)
	: QDialog(parent),m_len(3),m_curwaveIndex(0),m_isfilt(false),
	m_isdecom(false)
{
	ui.setupUi(this);
//	ui.waveplotter->setStyleSheet("QWidget{background-color:black}");
	wavepro = new WavePro;
	filter = new Filter1d;
}


void WaveProDlg::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
	if(!fileName.isEmpty())
	{
		loadFile(fileName);
	}

}


void WaveProDlg::loadFile(QString filename)
{
	if (!liblas::Open(m_ifs,filename.toStdString()))
	{
		throw std::runtime_error(std::string("Can not open file"));
	}
	lasreader = new liblas::Reader(m_ifs);

	size_t m_wave_num = lasreader->GetWaveRecordsCount();

	ui.nextButton->setEnabled(true);
	ui.horizontalSlider->setRange(0, m_wave_num);
	ui.spinBox->setRange(0, m_wave_num);
	readCurdata(m_curData);
	setPlotdata(0, m_plotData);
}

void WaveProDlg::readCurdata(QVector<QPointF> &data)
{
	m_plotData.clear();
	data.clear();

	lasreader->ReadWaveAt(m_curwaveIndex);
	std::vector<uint8_t> wave_data = lasreader->GetWave();
	wavepro->setInputData(wave_data);
	m_curwave = wavepro->getInputData();

	for (size_t i = 0; i < wave_data.size(); ++i) {
		data.append(QPointF(i,wave_data[i]));
	}
	m_plotData.push_back(data);
}

void WaveProDlg::setPlotdata(int index,const QVector< QVector<QPointF>> &data)
{
	ui.waveplotter->setCurveData(index,data);
}
void WaveProDlg::nextClicked()
{
	++m_curwaveIndex;
	readCurdata(m_curData);
	ui.spinBox->setValue(m_curwaveIndex);
	updatePlotData();

}

void WaveProDlg::previousClicked()
{
	--m_curwaveIndex;
    readCurdata(m_curData);
	ui.spinBox->setValue(m_curwaveIndex);
	updatePlotData();
}

void WaveProDlg::updatePlotData()
{	
	setPlotdata(0,m_plotData);
	//刷新滤波数据
	if(m_isfilt)
	{
		QVector< QVector<QPointF> > plotdata;
		vector<double> filtwave = m_curwave;
		filter->setInputData(m_curwave);
		if(ui.midradioButton->isChecked()){
			filter->filt(filtwave,MID,3);
			m_filttype = MID;
			m_len = 3;
			m_filtData.clear();
			for(int i = 0; i < m_curwave.size(); ++i){
				m_filtData.append(QPointF(i,filtwave[i]));
			}
			plotdata.push_back(m_filtData);
		} else if(ui.meanradioButton->isChecked()){
			filter->filt(filtwave,MEAN,3);
			m_filttype = MEAN;
			m_len = 3;
			m_filtData.clear();
			for(int i = 0; i < m_curwave.size(); ++i){
				m_filtData.append(QPointF(i,filtwave[i]));
			}
			plotdata.push_back(m_filtData);
		} else if(ui.gaussianradioButton->isChecked()){
			filter->filt(filtwave,GAUSSIAN,5,4);
			m_filttype = GAUSSIAN;
			m_len = 5;
			m_filtData.clear();
			for(int i = 0; i < m_curwave.size(); ++i){
				m_filtData.append(QPointF(i,filtwave[i]));
			}
			plotdata.push_back(m_filtData);
		} else if(ui.sgradioButton->isChecked()){
			filter->filt(filtwave,SG,7,3);
			m_filttype = SG;
			m_len = 7;
			m_k = 3;
			m_filtData.clear();
			for(int i = 0; i < m_curwave.size(); ++i){
				m_filtData.append(QPointF(i,filtwave[i]));
			}
			plotdata.push_back(m_filtData);
		}
		setPlotdata(1,plotdata);
	} else if(ui.nofilterradioButton->isChecked()){
		m_filttype = NON;
		int numofcurve = ui.waveplotter->getCurveNum();
		if(numofcurve>1){
           ui.waveplotter->clearCurve(1);
		}	
	}
	//更新分解数据
	if(m_isdecom){
		vector<Wave_Param> paramseq;
		wavepro->setPeakThreshold(5);
		wavepro->setPulseWidth(4);
		wavepro->setSampleSpacing(1000);
		if(m_filttype == GAUSSIAN)
		    wavepro->preprocess(m_filttype,m_len,m_sigma);
		else if(m_filttype == SG)
			wavepro->preprocess(m_filttype,m_len,m_k);
		else
			wavepro->preprocess(m_filttype,m_len);
		wavepro->initEstimateParam(paramseq);
		m_curparam = paramseq;
		QVector< QVector<QPointF> > plotdata1;

		if(ui.initdecomcheckBox->isChecked()){
			calwavecomdata(paramseq,plotdata1);
			setPlotdata(2,plotdata1);
		}else{
			ui.waveplotter->clearCurve(2);
		}

		if(ui.intiopticheckBox->isChecked()){
			int numofcurve = ui.waveplotter->getCurveNum();
			if(numofcurve>1){
				ui.waveplotter->clearCurve(2);
			}
			wavepro->removeBadParam(paramseq);
			m_curparam = paramseq;
			calwavecomdata(paramseq,plotdata1);
			setPlotdata(3,plotdata1);
		}else{
			ui.waveplotter->clearCurve(3);
		}

		if(ui.lmopticheckBox->isChecked()){
			ui.intiopticheckBox->setCheckState(Qt::Checked);
			ui.waveplotter->clearCurve(3);

			wavepro->optimizeParamWithLM(paramseq);
			m_curparam = paramseq;
			calwavecomdata(paramseq,plotdata1);
			setPlotdata(4,plotdata1);
		}else{
			ui.waveplotter->clearCurve(4);
		}

	}
}

void WaveProDlg::calwavecomdata(const vector<Wave_Param> &paramseq,QVector< QVector<QPointF> > &data)
{
	data.clear();
	vector<double> decomdata=m_curwave;

	double bnoise = wavepro->getBackgroundNoise();
	
	for(size_t k = 0; k < paramseq.size(); ++k){
		QVector<QPointF> datap;
		for(size_t i = 0; i < decomdata.size(); ++i){
			decomdata[i] = bnoise + paramseq[k].a * exp(-(double)(i - paramseq[k].b) * (double)(i - paramseq[k].b)/(2*paramseq[k].sigma * paramseq[k].sigma));
			datap.append(QPointF(i,decomdata[i]));
		}
		data.append(datap);
	}
}
void WaveProDlg::noFilterClicked()
{
	m_isfilt = false;
	updatePlotData();
}

void WaveProDlg::midFilterClicked()
{
	m_isfilt = true;
	updatePlotData();
}

void WaveProDlg::meanFilterClicked()
{
	m_isfilt = true;
	updatePlotData();
}
void WaveProDlg::gaussianFilterClicked()
{
	m_isfilt = true;
	updatePlotData();
}
void WaveProDlg::sgFilter_Clicked()
{
	m_isfilt = true;
	updatePlotData();
}
void WaveProDlg::initdecom_stateChange()
{

	m_isdecom = true;

	updatePlotData();
}
void WaveProDlg::initopti_stateChange()
{

	m_isdecom = true;
	updatePlotData();
}
void WaveProDlg::lmopti_stateChange()
{
	m_isdecom = true;
	updatePlotData();
}
void WaveProDlg::on_valueChange()
{
	m_curwaveIndex = ui.horizontalSlider->value();
	readCurdata(m_curData);
	updatePlotData();
}
void WaveProDlg::outputwavebutton_Clicked()
{
	QString filename;
	QString saveasfilename;
	filename.setNum(m_curwaveIndex);
	QString filedir = QString("/%1").arg(filename);

	saveasfilename = QFileDialog::getSaveFileName(this,tr("Save to text file"),filedir,tr("txt Files(*.txt)"));
	if(!saveasfilename.isEmpty()){
		savewave2txtFile(saveasfilename);
	}
}
void WaveProDlg::savewave2txtFile(QString filename)
{
	QFile file(filename);
	if(!file.open(QFile::WriteOnly|QFile::Text)){
		QMessageBox::critical(this,"critical","can not write the file");
	} else {
		QTextStream out(&file);

		for(int i = 0; i < m_curwave.size(); ++i){
			out << " " <<m_curwave[i];

		}
	}

}
void WaveProDlg::outputptbutton_Clicked()
{
	QString filename;
	QString saveasfilename;
	filename.setNum(m_curwaveIndex);
	QString filedir = QString("/%1").arg(filename);

	saveasfilename = QFileDialog::getSaveFileName(this,tr("Save to text file"),filedir,tr("txt Files(*.txt)"));
	if(!saveasfilename.isEmpty()){
		savept2txtFile(saveasfilename);
	}

}
void WaveProDlg::savept2txtFile(QString filename)
{
	QFile file(filename);
	if(!file.open(QFile::WriteOnly|QFile::Text)){
		QMessageBox::critical(this,"critical","can not write the file");
	} else {
		QTextStream out(&file);

		for (const auto& param: m_curparam)
		{
			out << param.a << " " << param.b << " " << param.sigma << "\n";
		}

	}
}
void WaveProDlg::on_sliderChange()
{

}
WaveProDlg::~WaveProDlg()
{

}
