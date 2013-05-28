#ifndef POLORGLCMBLOCK_H
#define POLORGLCMBLOCK_H

#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
class MatrixData;
class DesignNetSpace;
class DoubleRangeProperty;
}

namespace FlowerFeatureExtraction{

/*!
 *	\brief Gray-level co-occurrence matrix �Ҷȹ�������
 *  
 *  �ο�����2011[��������ͼ��Ļ�������ʶ�����о�]
 *	����ʹ�ü�����ĻҶȹ�������
 */
class PolorGLCMBlock : public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT

public:
	PolorGLCMBlock(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	~PolorGLCMBlock();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����

	/*!
	 *	\brief ����GLCM��6���������ο�����2011 
	 *	
	 *  feature[0]: ��Ե������ĸ���/����Ե�����㵽���ĵľ���֮�ͣ�,��Ӧ����2011�е�F[6]
	 *	feature[1]: glcm ����
	 *  feature[2]: glcm �Աȶ�
	 *	feature[3]: glcm ͬ����
	 *	feature[4]: ����sobel����֮��� glcm ���� 
	 *  feature[5]: ����sobel����֮��� glcm �Աȶ�
	 *  feature[6]: ����sobel����֮��� glcm ͬ����
	 */
	virtual bool process();     
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< ���ݵ���
	virtual void propertyChanged(DesignNet::Property *prop);
	bool connectionTest( DesignNet::Port* src, DesignNet::Port* target );

	void centroid();	//!<	������������
	void normalize(cv::Mat &glcm);	//!<	��һ��glcm
	float energe(const cv::Mat &glcm);//!< ����Ҷȹ������������
	float contrast(const cv::Mat &glcm);//!<  ����Աȶ�
	float homogeneity(const cv::Mat &glcm);//!< ����ͬ���ԣ�GLCM��Ԫ�ضԽ��߷ֲ��Ŀؼ����ܶȵĶ���

private:
	DesignNet::Port m_outputPort;	//!< ����˿�
	DesignNet::Port m_inputPort;	//!< ����˿�
	DesignNet::DoubleRangeProperty* m_doubleRangeProperty;
	DesignNet::DoubleRangeProperty* m_binCountProperty;//!< �Ҷȼ���Ĭ��Ϊ8
	cv::Mat m_glcm;					//!< �Ҷȹ�������
	cv::Mat m_glcm_sobel;			//!< sobel�˲���ĻҶȹ�������
	cv::Point2d m_centroid;			//!< ͼ����������
};
}
#endif // POLORGLCMBLOCK_H