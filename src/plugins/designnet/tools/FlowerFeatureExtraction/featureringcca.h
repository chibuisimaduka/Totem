#ifndef FEATURERINGACC_H
#define FEATURERINGACC_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
	class MatrixData;
}

namespace FlowerFeatureExtraction{
// 外连通环连通成分个数
class FeatureRingCCA : public DesignNet::Processor
{
	Q_OBJECT

public:
	FeatureRingCCA(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~FeatureRingCCA();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);

private:
	
	DesignNet::Port m_inputBinaryImagePort;	//!< 输入二值图端口
	DesignNet::Port m_inputCentroidPort;	//!< 输入中心位置端口

};

}

#endif // FEATURERINGACC_H
