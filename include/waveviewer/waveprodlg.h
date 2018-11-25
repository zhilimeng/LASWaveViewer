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
	FILT_TYPE  m_filttype;               //�˲�����
	unsigned int m_len;               //�˲����ڴ�С
	double m_sigma;                   //��˹�˲����
	double m_k;                       //SG�˲� ����ʽ����
	unsigned int m_curwaveIndex;      //��ǰ��������
	bool m_isfilt;                      //�Ƿ��˲�
	bool m_isdecom;                 //�Ƿ��ʼ�ֽ�
	size_t m_wave_num;              //���������ܸ���


	liblas::Reader *lasreader;
	std::ifstream m_ifs;
	WavePro *wavepro;
	Filter1d *filter;
	vector<double> m_curwave;        //��ǰ��������
	vector<Wave_Param> m_curparam;     //��ǰ���β���
	QVector<QPointF> m_curData;        //��ǰ��������
	QVector<QPointF> m_filtData;       //�˲�����
	QVector< QVector<QPointF> > m_plotData; //��������



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
