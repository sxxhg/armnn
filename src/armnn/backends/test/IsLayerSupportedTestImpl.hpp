//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//
#pragma once

#include "Graph.hpp"

#include <boost/core/ignore_unused.hpp>

namespace
{
armnn::Graph dummyGraph;

// Make a dummy TensorInfo object
template<armnn::DataType DataType>
armnn::TensorInfo MakeDummyTensorInfo()
{
    return armnn::TensorInfo({2,2,2,2}, DataType);
}


// Make a dummy WorkloadInfo using a dummy TensorInfo.
template<armnn::DataType DataType>
armnn::WorkloadInfo MakeDummyWorkloadInfo(unsigned int numInputs, unsigned int numOutputs)
{
    armnn::WorkloadInfo info;
    for (unsigned int i=0; i < numInputs; i++)
    {
        info.m_InputTensorInfos.push_back(MakeDummyTensorInfo<DataType>());
    }
    for (unsigned int o=0; o < numOutputs; o++)
    {
        info.m_OutputTensorInfos.push_back(MakeDummyTensorInfo<DataType>());
    }
    return info;
}

// template class to create a dummy layer (2 parameters)
template<typename LayerType, typename DescType = typename LayerType::DescriptorType>
struct DummyLayer
{
    DummyLayer()
    {
        m_Layer = dummyGraph.AddLayer<LayerType>(DescType(), "");
    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    LayerType* m_Layer;
};

// template class to create a dummy layer (1 parameter)
template<typename LayerType>
struct DummyLayer<LayerType, void>
{
    DummyLayer()
    {
        m_Layer = dummyGraph.AddLayer<LayerType>("");
    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    LayerType* m_Layer;
};

template<>
struct DummyLayer<armnn::ConstantLayer, void>
{
    DummyLayer()
    {
        m_Layer = dummyGraph.AddLayer<armnn::ConstantLayer>(std::shared_ptr<armnn::ScopedCpuTensorHandle>(), "");
    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    armnn::ConstantLayer* m_Layer;
};

template<>
struct DummyLayer<armnn::InputLayer, armnn::LayerBindingId>
{
    DummyLayer()
    {
        m_Layer = dummyGraph.AddLayer<armnn::InputLayer>(armnn::LayerBindingId(), "");

    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    armnn::InputLayer* m_Layer;
};

template<>
struct DummyLayer<armnn::MergerLayer>
{
    DummyLayer()
    {
        armnn::OriginsDescriptor desc(2);
        m_Layer = dummyGraph.AddLayer<armnn::MergerLayer>(desc, "");

    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    armnn::MergerLayer* m_Layer;
};

template<>
struct DummyLayer<armnn::OutputLayer, armnn::LayerBindingId>
{
    DummyLayer()
    {
        m_Layer = dummyGraph.AddLayer<armnn::OutputLayer>(armnn::LayerBindingId(), "");

    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    armnn::OutputLayer* m_Layer;
};

template<>
struct DummyLayer<armnn::SplitterLayer>
{
    DummyLayer()
    {
        armnn::ViewsDescriptor desc(1);
        m_Layer = dummyGraph.AddLayer<armnn::SplitterLayer>(desc, "");

    }
    ~DummyLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    armnn::SplitterLayer* m_Layer;
};

template <typename ConvolutionLayerType>
struct DummyConvolutionLayer
{
    DummyConvolutionLayer()
    {
        typename ConvolutionLayerType::DescriptorType desc;
        m_Layer = dummyGraph.AddLayer<ConvolutionLayerType>(desc, "");
        m_Layer->m_Weight = std::make_unique<armnn::ScopedCpuTensorHandle>(
            armnn::TensorInfo(armnn::TensorShape({1,1,1,1}), armnn::DataType::Float32));
        m_Layer->m_Bias = std::make_unique<armnn::ScopedCpuTensorHandle>(
            armnn::TensorInfo(armnn::TensorShape({1,1,1,1}), armnn::DataType::Float32));
    }
    ~DummyConvolutionLayer()
    {
        dummyGraph.EraseLayer(m_Layer);
    }
    ConvolutionLayerType* m_Layer;
};

template<>
struct DummyLayer<armnn::Convolution2dLayer>
    : public DummyConvolutionLayer<armnn::Convolution2dLayer>
{
};

template<>
struct DummyLayer<armnn::DepthwiseConvolution2dLayer>
    : public DummyConvolutionLayer<armnn::DepthwiseConvolution2dLayer>
{
};

// Tag for giving LayerType entries a unique strong type each.
template<armnn::LayerType>
struct Tag{};

#define DECLARE_LAYER_POLICY_CUSTOM_PARAM(name, descType) \
template<armnn::DataType DataType> \
struct LayerTypePolicy<armnn::LayerType::name, DataType> \
{ \
    using Type = armnn::name##Layer; \
    using Desc = descType; \
    using QueueDesc = armnn::name##QueueDescriptor; \
    constexpr static const char* NameStr = #name; \
    \
    static std::unique_ptr<armnn::IWorkload> MakeDummyWorkload(armnn::IWorkloadFactory *factory, \
        unsigned int nIn, unsigned int nOut) \
    { \
        QueueDesc desc; \
        armnn::WorkloadInfo info = MakeDummyWorkloadInfo<DataType>(nIn, nOut); \
        return factory->Create##name(desc, info); \
    } \
};

// define a layer policy specialization for use with the IsLayerSupported tests.
// Use this version for layers whose constructor takes 1 parameter(name).
#define DECLARE_LAYER_POLICY_1_PARAM(name) DECLARE_LAYER_POLICY_CUSTOM_PARAM(name, void)

// define a layer policy specialization for use with the IsLayerSupported tests.
// Use this version for layers whose constructor takes 2 parameters(descriptor and name).
#define DECLARE_LAYER_POLICY_2_PARAM(name) DECLARE_LAYER_POLICY_CUSTOM_PARAM(name, armnn::name##Descriptor)

// Layer policy template
template<armnn::LayerType Type, armnn::DataType DataType>
struct LayerTypePolicy;

// Every entry in the armnn::LayerType enum must be accounted for below.
DECLARE_LAYER_POLICY_2_PARAM(Activation)

DECLARE_LAYER_POLICY_1_PARAM(Addition)

DECLARE_LAYER_POLICY_2_PARAM(BatchNormalization)

DECLARE_LAYER_POLICY_1_PARAM(Constant)

DECLARE_LAYER_POLICY_2_PARAM(Convolution2d)

DECLARE_LAYER_POLICY_1_PARAM(MemCopy)

DECLARE_LAYER_POLICY_2_PARAM(DepthwiseConvolution2d)

DECLARE_LAYER_POLICY_2_PARAM(FakeQuantization)

DECLARE_LAYER_POLICY_1_PARAM(Floor)

DECLARE_LAYER_POLICY_2_PARAM(FullyConnected)

DECLARE_LAYER_POLICY_CUSTOM_PARAM(Input, armnn::LayerBindingId)

DECLARE_LAYER_POLICY_1_PARAM(L2Normalization)

DECLARE_LAYER_POLICY_2_PARAM(Merger)

DECLARE_LAYER_POLICY_1_PARAM(Multiplication)

DECLARE_LAYER_POLICY_2_PARAM(Normalization)

DECLARE_LAYER_POLICY_CUSTOM_PARAM(Output, armnn::LayerBindingId)

DECLARE_LAYER_POLICY_2_PARAM(Permute)

DECLARE_LAYER_POLICY_2_PARAM(Pooling2d)

DECLARE_LAYER_POLICY_2_PARAM(ResizeBilinear)

DECLARE_LAYER_POLICY_2_PARAM(Softmax)

DECLARE_LAYER_POLICY_2_PARAM(Splitter)

DECLARE_LAYER_POLICY_2_PARAM(Reshape)


// Generic implementation to get the number of input slots for a given layer type;
template<armnn::LayerType Type>
unsigned int GetNumInputs(const armnn::Layer& layer)
{
    return layer.GetNumInputSlots();
}

// Generic implementation to get the number of output slots for a given layer type;
template<armnn::LayerType Type>
unsigned int GetNumOutputs(const armnn::Layer& layer)
{
    return layer.GetNumOutputSlots();
}

template<>
unsigned int GetNumInputs<armnn::LayerType::Merger>(const armnn::Layer& layer)
{
    boost::ignore_unused(layer);
    return 2;
}

// Test that the IsLayerSupported() function returns the correct value.
// We determine the correct value by *trying* to create the relevant workload and seeing if it matches what we expect.
// Returns true if expectations are met, otherwise returns false.
template<typename FactoryType, armnn::DataType DataType, armnn::LayerType Type>
bool IsLayerSupportedTest(FactoryType *factory, Tag<Type>)
{
    using LayerPolicy = LayerTypePolicy<Type, DataType>;
    using LayerType = typename LayerPolicy::Type;
    using LayerDesc = typename LayerPolicy::Desc;
    DummyLayer<LayerType, LayerDesc> layer;

    unsigned int numIn = GetNumInputs<Type>(*layer.m_Layer);
    unsigned int numOut = GetNumOutputs<Type>(*layer.m_Layer);

    // Make another dummy layer just to make IsLayerSupported have valid inputs
    DummyLayer<armnn::ConstantLayer, void> previousLayer;
    // Set output of previous layer to a dummy tensor
    armnn::TensorInfo output = MakeDummyTensorInfo<DataType>();
    previousLayer.m_Layer->GetOutputSlot(0).SetTensorInfo(output);
    // Connect all outputs of previous layer to inputs of tested layer
    for (unsigned int i = 0; i < numIn; i++)
    {
        armnn::IOutputSlot& previousLayerOutputSlot = previousLayer.m_Layer->GetOutputSlot(0);
        armnn::IInputSlot& layerInputSlot = layer.m_Layer->GetInputSlot(i);
        previousLayerOutputSlot.Connect(layerInputSlot);
    }
    // Set outputs of tested layer to a dummy tensor
    for (unsigned int i = 0; i < numOut; i++)
    {
        layer.m_Layer->GetOutputSlot(0).SetTensorInfo(output);
    }

    std::string layerName = LayerPolicy::NameStr;
    std::string reasonIfUnsupported;
    if (FactoryType::IsLayerSupported(*layer.m_Layer, DataType, reasonIfUnsupported))
    {
        std::string errorMsg = " layer expected support but found none.";
        try
        {
            bool retVal = LayerPolicy::MakeDummyWorkload(factory, numIn, numOut).get() != nullptr;
            BOOST_CHECK_MESSAGE(retVal, layerName << errorMsg);
            return retVal;
        }
        catch (const armnn::InvalidArgumentException& e)
        {
            boost::ignore_unused(e);
            // This is ok since we throw InvalidArgumentException when creating the dummy workload.
            return true;
        }
        catch(const std::exception& e)
        {
            errorMsg = e.what();
            BOOST_TEST_ERROR(layerName << ": " << errorMsg);
            return false;
        }
        catch (...)
        {
            errorMsg = "Unexpected error while testing support for ";
            BOOST_TEST_ERROR(errorMsg << layerName);
            return false;
        }
    }
    else
    {
        std::string errorMsg = "layer expected no support (giving reason: " + reasonIfUnsupported + ") but found some.";
        try
        {
            bool retVal = LayerPolicy::MakeDummyWorkload(factory, numIn, numOut).get() == nullptr;
            BOOST_CHECK_MESSAGE(retVal, layerName << errorMsg);
            return retVal;
        }
        // These two exceptions are ok: For workloads that are partially supported, attempting to instantiate them
        // using parameters that make IsLayerSupported() return false should throw an
        // InvalidArgumentException or UnimplementedException
        catch(const armnn::InvalidArgumentException& e)
        {
            boost::ignore_unused(e);
            return true;
        }
        catch (const armnn::UnimplementedException& e)
        {
            boost::ignore_unused(e);
            return true;
        }
        catch(const std::exception& e)
        {
            errorMsg = e.what();
            BOOST_TEST_ERROR(layerName << ": " << errorMsg);
            return false;
        }
        catch (...)
        {
            errorMsg = "Unexpected error while testing support for ";
            BOOST_TEST_ERROR(errorMsg << layerName);
            return false;
        }
    }
}

// Helper function to compute the next type in the LayerType enum
constexpr armnn::LayerType NextType(armnn::LayerType type)
{
    return static_cast<armnn::LayerType>(static_cast<int>(type)+1);
}

// Termination function for determining the end of the LayerType enumeration
template<typename FactoryType, armnn::DataType DataType, armnn::LayerType Type>
bool IsLayerSupportedTestsImpl(FactoryType *factory, Tag<armnn::LayerType::LastLayer>)
{
    return IsLayerSupportedTest<FactoryType, DataType, Type>(factory, Tag<Type>());
};

// Recursive function to test and entry in the LayerType enum and then iterate on the next entry.
template<typename FactoryType, armnn::DataType DataType, armnn::LayerType Type>
bool IsLayerSupportedTestsImpl(FactoryType *factory, Tag<Type>)
{
    bool v = IsLayerSupportedTest<FactoryType, DataType, Type>(factory, Tag<Type>());

    return v &&
    IsLayerSupportedTestsImpl<FactoryType, DataType, NextType(Type)>
        (factory, Tag<NextType(Type)>());
};

// Helper function to pass through to the test framework.
template<typename FactoryType, armnn::DataType DataType>
bool IsLayerSupportedTests(FactoryType *factory)
{
    return IsLayerSupportedTestsImpl<FactoryType, DataType>(factory, Tag<armnn::LayerType::FirstLayer>());
};

template<armnn::LayerType Type>
bool TestLayerTypeMatches()
{
    using LayerPolicy = LayerTypePolicy<Type, armnn::DataType::Float32>;
    using LayerType = typename LayerPolicy::Type;
    using LayerDesc = typename LayerPolicy::Desc;
    DummyLayer<LayerType, LayerDesc> layer;

    std::stringstream ss;
    ss << LayerPolicy::NameStr << " layer type mismatches expected layer type value.";
    bool v = Type == layer.m_Layer->GetType();
    BOOST_CHECK_MESSAGE(v, ss.str());
    return v;
};

template<armnn::LayerType Type>
bool LayerTypeMatchesTestImpl(Tag<armnn::LayerType::LastLayer>)
{
    return TestLayerTypeMatches<Type>();
};

template<armnn::LayerType Type>
bool LayerTypeMatchesTestImpl(Tag<Type>)
{
    return TestLayerTypeMatches<Type>() &&
        LayerTypeMatchesTestImpl<NextType(Type)>(Tag<NextType(Type)>());
};

bool LayerTypeMatchesTest()
{
    return LayerTypeMatchesTestImpl<armnn::LayerType::FirstLayer>(Tag<armnn::LayerType::FirstLayer>());
};

} //namespace
