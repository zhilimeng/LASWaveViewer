#ifndef WAVEPRODLG_H
#define WAVEPRODLG_H

#include <QtWidgets/QDialog>
#include <QtWidgets>

// liblas
#include <liblas/liblas.hpp>
#include <liblas/point.hpp>
#include <liblas/reader.hpp>

#include "ui_waveprodlg.h"
#include "wavepro.h"
#include "filter1d.h"
using namespace std;
class WaveProDlg : public QDialog
{
	Q_OBJECT

public:
	WaveProDlg(QWidget *parent = 0);
	~WaveProDlg();

	void updatePlotData();
	void setPlotdata(int index,const QVector< QVector<QPointF>> &data);
	void calwavecomdata(const vector<Wave_Param> &paramseq,QVector< QVector<QPointF> > &data);
	void readCurdata(QVector<QPointF> &data);

private:
	void loadFile(QString filename);
	void savewave2txtFile(QString filename);
	void savept2txtFile(QString filename);
	QString fileName;
	FILT_TYPE  m_filttype;               //滤波类型
	unsigned int m_len;               //滤波窗口大小
	double m_sigma;                   //高斯滤波宽度
	double m_k;                       //SG滤波 多项式阶数
	unsigned int m_curwaveIndex;      //当前波形索引
	bool m_isfilt;                      //是否滤波
	bool m_isdecom;                 //是否初始分解
	size_t m_wave_num;              //波形数据总个数


	liblas::Reader *lasreader;
	std::ifstream m_ifs;
	WavePro *wavepro;
	Filter1d *filter;
	vector<double> m_curwave;        //当前波形数据
	vector<Wave_Param> m_curparam;     //当前波形参数
	QVector<QPointF> m_curData;        //当前绘制数据
	QVector<QPointF> m_filtData;       //滤波数据
	QVector< QVector<QPointF> > m_plotData; //绘制数据



private slots:
	void openFile();
	void nextClicked();
	void previousClicked();
	void noFilterClicked();
	void midFilterClicked();
	void meanFilterClicked();
	void gaussianFilterClicked();
	void sgFilter_Clicked();
	void initdecom_stateChange();
	void initopti_stateChange();
	void lmopti_stateChange();
	void on_valueChange();
	void on_sliderChange();
	void outputptbutton_Clicked();
	void outputwavebutton_Clicked();


private:
	Ui::WaveProDlgClass ui;
};

#endif // WAVEPRODLG_H
