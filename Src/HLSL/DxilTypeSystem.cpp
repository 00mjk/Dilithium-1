/**
 * @file DxilTypeSystem.cpp
 * @author Minmin Gong
 *
 * @section DESCRIPTION
 *
 * This source file is part of Dilithium
 * For the latest info, see https://github.com/gongminmin/Dilithium
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Minmin Gong. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Dilithium/Dilithium.hpp>
#include <Dilithium/DerivedType.hpp>
#include <Dilithium/dxc/HLSL/DxilTypeSystem.hpp>

namespace Dilithium
{
	DxilTypeSystem::DxilTypeSystem(LLVMModule* mod)
		: module_(mod)
	{
	}

	DxilStructAnnotation* DxilTypeSystem::AddStructAnnotation(StructType const * struct_type)
	{
		DILITHIUM_UNUSED(struct_type);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	DxilStructAnnotation* DxilTypeSystem::GetStructAnnotation(StructType const * struct_type)
	{
		DILITHIUM_UNUSED(struct_type);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	void DxilTypeSystem::EraseStructAnnotation(StructType const * struct_type)
	{
		DILITHIUM_UNUSED(struct_type);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	DxilFunctionAnnotation* DxilTypeSystem::AddFunctionAnnotation(Function const * function)
	{
		BOOST_ASSERT(function_annotations_.find(function) == function_annotations_.end());

		auto anno = std::make_unique<DxilFunctionAnnotation>();
		auto ret = anno.get();
		function_annotations_[function] = std::move(anno);
		ret->function_ = function;
		ret->parameter_annotations_.resize(function->GetFunctionType()->NumParams());
		return ret;
	}

	DxilFunctionAnnotation* GetFunctionAnnotation(Function const * function)
	{
		DILITHIUM_UNUSED(function);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	void EraseFunctionAnnotation(Function* const function)
	{
		DILITHIUM_UNUSED(function);

		DILITHIUM_NOT_IMPLEMENTED;
	}


	uint32_t DxilStructAnnotation::NumFields() const
	{
		DILITHIUM_NOT_IMPLEMENTED;
	}

	DxilFieldAnnotation& DxilStructAnnotation::FieldAnnotation(uint32_t index)
	{
		DILITHIUM_UNUSED(index);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	DxilFieldAnnotation const & DxilStructAnnotation::FieldAnnotation(uint32_t index) const
	{
		DILITHIUM_UNUSED(index);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	StructType const * DxilStructAnnotation::GetStructType() const
	{
		DILITHIUM_NOT_IMPLEMENTED;
	}

	uint32_t DxilStructAnnotation::CBufferSize() const
	{
		DILITHIUM_NOT_IMPLEMENTED;
	}

	void DxilStructAnnotation::CBufferSize(uint32_t size)
	{
		DILITHIUM_UNUSED(size);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	void DxilStructAnnotation::MarkEmptyStruct()
	{
		DILITHIUM_NOT_IMPLEMENTED;
	}

	bool DxilStructAnnotation::IsEmptyStruct()
	{
		DILITHIUM_NOT_IMPLEMENTED;
	}


	DxilParameterAnnotation& DxilFunctionAnnotation::ParameterAnnotation(uint32_t index)
	{
		DILITHIUM_UNUSED(index);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	DxilParameterAnnotation const & DxilFunctionAnnotation::ParameterAnnotation(uint32_t index) const
	{
		DILITHIUM_UNUSED(index);

		DILITHIUM_NOT_IMPLEMENTED;
	}

	Function const * DxilFunctionAnnotation::GetFunction() const
	{
		DILITHIUM_NOT_IMPLEMENTED;
	}
}
