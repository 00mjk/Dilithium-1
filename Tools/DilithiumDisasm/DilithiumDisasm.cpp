/**
 * @file DilithiumDisasm.cpp
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

#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

#include <Dilithium/Dilithium.hpp>

#include <Dilithium/AsmWriter.hpp>
#include <Dilithium/Constants.hpp>
#include <Dilithium/DerivedType.hpp>
#include <Dilithium/Instructions.hpp>

#include <Dilithium/dxc/HLSL/DxilCBuffer.hpp>
#include <Dilithium/dxc/HLSL/DxilContainer.hpp>
#include <Dilithium/dxc/HLSL/DxilOperations.hpp>
#include <Dilithium/dxc/HLSL/DxilPipelineStateValidation.hpp>
#include <Dilithium/dxc/HLSL/DxilTypeSystem.hpp>
#include <Dilithium/dxc/HLSL/DxilModule.hpp>
#include <Dilithium/dxc/HLSL/DxilResource.hpp>
#include <Dilithium/dxc/HLSL/DxilSampler.hpp>
#include <Dilithium/dxc/HLSL/HLMatrixLowerHelper.hpp>

using namespace Dilithium;

namespace
{
	class DxcAssemblyAnnotationWriter : public AssemblyAnnotationWriter
	{
	public:
		void PrintInfoComment(Value const & v, std::ostream& os) override
		{
			static char const * OpCodeSignatures[] =
			{
				"(index)",  // TempRegLoad
				"(index,value)",  // TempRegStore
				"(regIndex,index,component)",  // MinPrecXRegLoad
				"(regIndex,index,component,value)",  // MinPrecXRegStore
				"(inputSigId,rowIndex,colIndex,gsVertexAxis)",  // LoadInput
				"(outputtSigId,rowIndex,colIndex,value)",  // StoreOutput
				"(value)",  // FAbs
				"(value)",  // Saturate
				"(value)",  // IsNaN
				"(value)",  // IsInf
				"(value)",  // IsFinite
				"(value)",  // IsNormal
				"(value)",  // Cos
				"(value)",  // Sin
				"(value)",  // Tan
				"(value)",  // Acos
				"(value)",  // Asin
				"(value)",  // Atan
				"(value)",  // Hcos
				"(value)",  // Hsin
				"(value)",  // Exp
				"(value)",  // Frc
				"(value)",  // Log
				"(value)",  // Sqrt
				"(value)",  // Rsqrt
				"(value)",  // Round_ne
				"(value)",  // Round_ni
				"(value)",  // Round_pi
				"(value)",  // Round_z
				"(value)",  // Bfrev
				"(value)",  // Countbits
				"(value)",  // FirstbitLo
				"(value)",  // FirstbitHi
				"(value)",  // FirstbitSHi
				"(a,b)",  // FMax
				"(a,b)",  // FMin
				"(a,b)",  // IMax
				"(a,b)",  // IMin
				"(a,b)",  // UMax
				"(a,b)",  // UMin
				"(a,b)",  // IMul
				"(a,b)",  // UMul
				"(a,b)",  // UDiv
				"(a,b)",  // IAddc
				"(a,b)",  // UAddc
				"(a,b)",  // ISubc
				"(a,b)",  // USubc
				"(a,b,c)",  // FMad
				"(a,b,c)",  // Fma
				"(a,b,c)",  // IMad
				"(a,b,c)",  // UMad
				"(a,b,c)",  // Msad
				"(a,b,c)",  // Ibfe
				"(a,b,c)",  // Ubfe
				"(width,offset,value,replaceCount)",  // Bfi
				"(ax,ay,bx,by)",  // Dot2
				"(ax,ay,az,bx,by,bz)",  // Dot3
				"(ax,ay,az,aw,bx,by,bz,bw)",  // Dot4
				"(resourceClass,rangeId,index,nonUniformIndex)",  // CreateHandle
				"(handle,byteOffset,alignment)",  // CBufferLoad
				"(handle,regIndex)",  // CBufferLoadLegacy
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)",  // Sample
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,bias,clamp)",  // SampleBias
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)",  // SampleLevel
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,ddx0,ddx1,ddx2,ddy0,ddy1,ddy2,clamp)",  // SampleGrad
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,compareValue,clamp)",  // SampleCmp
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,compareValue)",  // SampleCmpLevelZero
				"(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)",  // TextureLoad
				"(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)",  // TextureStore
				"(srv,index,wot)",  // BufferLoad
				"(uav,coord0,coord1,value0,value1,value2,value3,mask)",  // BufferStore
				"(uav,inc)",  // BufferUpdateCounter
				"(status)",  // CheckAccessFullyMapped
				"(handle,mipLevel)",  // GetDimensions
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,channel)",  // TextureGather
				"(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,channel,compareVale)",  // TextureGatherCmp
				"()",  // ToDelete5
				"()",  // ToDelete6
				"(srv,index)",  // Texture2DMSGetSamplePosition
				"(index)",  // RenderTargetGetSamplePosition
				"()",  // RenderTargetGetSampleCount
				"(handle,atomicOp,offset0,offset1,offset2,newValue)",  // AtomicBinOp
				"(handle,offset0,offset1,offset2,compareValue,newValue)",  // AtomicCompareExchange
				"(barrierMode)",  // Barrier
				"(handle,sampler,coord0,coord1,coord2,clamped)",  // CalculateLOD
				"(condition)",  // Discard
				"(value)",  // DerivCoarseX
				"(value)",  // DerivCoarseY
				"(value)",  // DerivFineX
				"(value)",  // DerivFineY
				"(inputSigId,inputRowIndex,inputColIndex,offsetX,offsetY)",  // EvalSnapped
				"(inputSigId,inputRowIndex,inputColIndex,sampleIndex)",  // EvalSampleIndex
				"(inputSigId,inputRowIndex,inputColIndex)",  // EvalCentroid
				"(component)",  // ThreadId
				"(component)",  // GroupId
				"(component)",  // ThreadIdInGroup
				"()",  // FlattenedThreadIdInGroup
				"(streamId)",  // EmitStream
				"(streamId)",  // CutStream
				"(streamId)",  // EmitThenCutStream
				"(lo,hi)",  // MakeDouble
				"()",  // ToDelete1
				"()",  // ToDelete2
				"(value)",  // SplitDouble
				"()",  // ToDelete3
				"()",  // ToDelete4
				"(inputSigId,row,col,index)",  // LoadOutputControlPoint
				"(inputSigId,row,col)",  // LoadPatchConstant
				"(component)",  // DomainLocation
				"(outputSigID,row,col,value)",  // StorePatchConstant
				"()",  // OutputControlPointID
				"()",  // PrimitiveID
				"()",  // CycleCounterLegacy
				"(value)",  // Htan
				"()",  // WaveCaptureReserved
				"()",  // WaveIsFirstLane
				"()",  // WaveGetLaneIndex
				"()",  // WaveGetLaneCount
				"()",  // WaveIsHelperLaneReserved
				"(cond)",  // WaveAnyTrue
				"(cond)",  // WaveAllTrue
				"(value)",  // WaveActiveAllEqual
				"(cond)",  // WaveActiveBallot
				"(value,lane)",  // WaveReadLaneAt
				"(value)",  // WaveReadLaneFirst
				"(value,op,sop)",  // WaveActiveOp
				"(value,op)",  // WaveActiveBit
				"(value,op,sop)",  // WavePrefixOp
				"()",  // WaveGetOrderedIndex
				"()",  // GlobalOrderedCountIncReserved
				"(value,quadLane)",  // QuadReadLaneAt
				"(value,op)",  // QuadOp
				"(value)",  // BitcastI16toF16
				"(value)",  // BitcastF16toI16
				"(value)",  // BitcastI32toF32
				"(value)",  // BitcastF32toI32
				"(value)",  // BitcastI64toF64
				"(value)",  // BitcastF64toI64
				"()",  // GSInstanceID
				"(value)",  // LegacyF32ToF16
				"(value)",  // LegacyF16ToF32
				"(value)",  // LegacyDoubleToFloat
				"(value)",  // LegacyDoubleToSInt32
				"(value)",  // LegacyDoubleToUInt32
				"(value)",  // WaveAllBitCount
				"(value)",  // WavePrefixBitCount
				"()",  // SampleIndex
				"()",  // Coverage
				"()"  // InnerCoverage
			};

			auto ci = dyn_cast<CallInst const>(&v);
			if (!ci)
			{
				return;
			}
			// TODO: annotate high-level operations where possible as well
			if ((ci->NumArgOperands() == 0) || (ci->CalledFunction()->Name().find("dx.op.") != 0))
			{
				return;
			}
			auto cint = dyn_cast<ConstantInt const>(ci->ArgOperand(0));
			if (!cint)
			{
				// At this point, we know this is malformed; ignore.
				return;
			}

			uint32_t opcode_val = static_cast<uint32_t>(cint->ZExtValue());
			if (opcode_val >= static_cast<uint32_t>(OpCode::NumOpCodes))
			{
				os << "  ; invalid DXIL opcode #" << opcode_val;
				return;
			}

			// TODO: if an argument references a resource, look it up and write the
			// name/binding
			OpCode opcode = static_cast<OpCode>(opcode_val);
			os << "  ; " << OP::GetOpCodeName(opcode) << OpCodeSignatures[opcode_val];
		}
	};


	template <typename T>
	T const * ByteOffset(void const * p, uint32_t byte_offset)
	{
		return reinterpret_cast<T const *>(static_cast<uint8_t const *>(p) + byte_offset);
	}

	void PrintFeatureInfo(DxilShaderFeatureInfo const * feature_info, std::ostream& os, char const * comment)
	{
		static char const * feature_info_names[] =
		{
			"Double-precision floating point",
			"Raw and Structured buffers",
			"UAVs at every shader stage",
			"64 UAV slots",
			"Minimum-precision data types",
			"Double-precision extensions for 11.1",
			"Shader extensions for 11.1",
			"Comparison filtering for feature level 9",
			"Tiled resources",
			"PS Output Stencil Ref",
			"PS Inner Coverage",
			"Typed UAV Load Additional Formats",
			"Raster Ordered UAVs",
			"SV_RenderTargetArrayIndex or SV_ViewportArrayIndex from any shader feeding rasterizer",
			"Wave level operations",
			"64-Bit integer",
		};

		uint64_t feature_flags = feature_info->FeatureFlags;
		if (!feature_flags)
		{
			return;
		}
		os << comment << std::endl;
		os << comment << " Note: shader requires additional functionality:" << std::endl;
		for (uint32_t i = 0; i < DSFI_Count; ++ i)
		{
			if (feature_flags & (1ULL << i))
			{
				os << comment << "       " << feature_info_names[i] << std::endl;
			}
		}
		os << comment << std::endl;
	}

	void PrintSignature(char const * name, DxilProgramSignature const * signature, bool is_input, std::ostream& os, char const * comment)
	{
		static char const * sys_value_names[] =
		{
			"NONE",
			"POS",
			"CLIPDST",
			"CULLDST",
			"RTINDEX",
			"VPINDEX",
			"VERTID",
			"PRIMID",
			"INSTID",
			"FFACE",
			"SAMPLE",
			"QUADEDGE",
			"QUADINT",
			"TRIEDGE",
			"TRIINT",
			"LINEDET",
			"LINEDEN",
			"TARGET",
			"DEPTH",
			"COVERAGE",
			"DEPTHGE",
			"DEPTHLE",
			"STENCILREF",
			"INNERCOV"
		};

		static char const * comp_type_names[] =
		{
			"unknown",
			"uint",
			"int",
			"float",
			"min16u",
			"min16i",
			"min16f",
			"uint64",
			"int64",
			"double"
		};

		os << comment << std::endl
			<< comment << " " << name << " signature:" << std::endl
			<< comment << std::endl
			<< comment << " Name                 Index   Mask Register SysValue  Format   Used" << std::endl
			<< comment << " -------------------- ----- ------ -------- -------- ------- ------" << std::endl;

		if (signature->ParamCount == 0)
		{
			os << comment << " no parameters" << std::endl;
			return;
		}

		auto sig_beg = ByteOffset<DxilProgramSignatureElement>(signature, signature->ParamOffset);
		auto sig_end = sig_beg + signature->ParamCount;

		bool has_streams = std::any_of(sig_beg, sig_end,
			[](DxilProgramSignatureElement const & signature) { return signature.Stream != 0; });
		for (auto sig = sig_beg; sig != sig_end; ++ sig)
		{
			os << comment << " ";
			auto semantic_name = ByteOffset<char>(signature, sig->SemanticName);
			if (has_streams)
			{
				os << "m" << sig->Stream << ":";
				os << std::left << std::setw(17) << semantic_name;
			}
			else
			{
				os << std::left << std::setw(20) << semantic_name;
			}

			os << std::right << std::setw(6) << sig->SemanticIndex;

			if (sig->Register == -1)
			{
				os << "    N/A";
				if (!_stricmp(semantic_name, "SV_Depth"))
				{
					os << "   oDepth";
				}
				else if (0 == _stricmp(semantic_name, "SV_DepthGreaterEqual"))
				{
					os << " oDepthGE";
				}
				else if (0 == _stricmp(semantic_name, "SV_DepthLessEqual"))
				{
					os << " oDepthLE";
				}
				else if (0 == _stricmp(semantic_name, "SV_Coverage"))
				{
					os << "    oMask";
				}
				else if (0 == _stricmp(semantic_name, "SV_StencilRef"))
				{
					os << "    oStencilRef";
				}
				else if (sig->SystemValue == DxilProgramSigSemantic::PrimitiveID)
				{
					os << "   primID";
				}
				else
				{
					os << "  special";
				}
			}
			else
			{
				os << "   ";
				for (int i = 0; i < 4; ++ i)
				{
					if (sig->Mask & (1UL << i))
					{
						os << "xyzw"[i];
					}
					else
					{
						os << ' ';
					}
				}
				os << std::right << std::setw(9) << sig->Register;
			}

			os << std::right << std::setw(9) << sys_value_names[static_cast<uint32_t>(sig->SystemValue)];
			os << std::right << std::setw(8) << comp_type_names[static_cast<uint32_t>(sig->CompType)];

			uint8_t rw_mask = sig->AlwaysReads_Mask;
			if (!is_input)
			{
				rw_mask = ~rw_mask;
			}

			if (sig->Register == -1)
			{
				os << (rw_mask ? "    YES" : "     NO");
			}
			else
			{
				os << "   ";
				for (int i = 0; i < 4; ++ i)
				{
					if (rw_mask & (1UL << i))
					{
						os << "xyzw"[i];
					}
					else
					{
						os << ' ';
					}
				}
			}

			os << std::endl;
		}
		os << comment << std::endl;
	}

	void PrintPipelineStateValidationRuntimeInfo(char const * buff, ShaderKind shader_kind, std::ostream& os, char const * comment)
	{
		static char const * input_primitive_names[] =
		{
			"invalid",
			"point",
			"line",
			"triangle",
			"invalid",
			"invalid",
			"lineadj",
			"triangleadj",
			"patch1",
			"patch2",
			"patch3",
			"patch4",
			"patch5",
			"patch6",
			"patch7",
			"patch8",
			"patch9",
			"patch10",
			"patch11",
			"patch12",
			"patch13",
			"patch14",
			"patch15",
			"patch16",
			"patch17",
			"patch18",
			"patch19",
			"patch20",
			"patch21",
			"patch22",
			"patch23",
			"patch24",
			"patch25",
			"patch26",
			"patch27",
			"patch28",
			"patch29",
			"patch30",
			"patch31",
			"patch32"
		};

		static char const * primitive_topology_names[] =
		{
			"invalid",
			"point",
			"invalid",
			"line",
			"invalid",
			"triangle"
		};

		static char const * tessellator_domain_names[] =
		{
			"invalid",
			"isoline",
			"tri",
			"quad"
		};

		static char const * tessellator_output_primitive_names[] =
		{
			"invalid",
			"point",
			"line",
			"triangle_cw",
			"triangle_ccw"
		};

		os << comment << std::endl
			<< comment << " Pipeline Runtime Information:" << std::endl
			<< comment << std::endl;

		uint32_t const offset = sizeof(uint32_t);
		auto info = reinterpret_cast<PSVRuntimeInfo0 const *>(buff + offset);

		switch (shader_kind)
		{
		case ShaderKind::Vertex:
			os << comment << " Vertex Shader" << std::endl;
			os << comment << " OutputPositionPresent=" << (info->VS.OutputPositionPresent ? true : false) << std::endl;
			break;

		case ShaderKind::Pixel:
			os << comment << " Pixel Shader" << std::endl;
			os << comment << " DepthOutput=" << (info->PS.DepthOutput ? true : false) << std::endl;
			os << comment << " SampleFrequency=" << (info->PS.SampleFrequency ? true : false) << std::endl;
			break;

		case ShaderKind::Geometry:
			os << comment << " Geometry Shader" << std::endl;
			os << comment << " InputPrimitive=" << input_primitive_names[info->GS.InputPrimitive] << std::endl;
			os << comment << " OutputTopology=" << primitive_topology_names[info->GS.OutputTopology] << std::endl;
			os << comment << " OutputStreamMask=" << info->GS.OutputStreamMask << std::endl;
			os << comment << " OutputPositionPresent=" << (info->GS.OutputPositionPresent ? true : false) << std::endl;
			break;

		case ShaderKind::Hull:
			os << comment << " Hull Shader" << std::endl;
			os << comment << " InputControlPointCount=" << info->HS.InputControlPointCount << std::endl;
			os << comment << " OutputControlPointCount=" << info->HS.OutputControlPointCount << std::endl;
			os << comment << " Domain=" << tessellator_domain_names[info->HS.TessellatorDomain] << std::endl;
			os << comment << " OutputPrimitive=" << tessellator_output_primitive_names[info->HS.TessellatorOutputPrimitive] << std::endl;
			break;

		case ShaderKind::Domain:
			os << comment << " Domain Shader" << std::endl;;
			os << comment << " InputControlPointCount=" << info->DS.InputControlPointCount << std::endl;
			os << comment << " OutputPositionPresent=" << (info->DS.OutputPositionPresent ? true : false) << std::endl;
			break;
		}

		os << comment << std::endl;
	}

	void PrintDxilSignature(char const * name, DxilSignature const & signature, std::ostream& os, char const * comment)
	{
		auto const & sig_elts = signature.Elements();
		if (sig_elts.empty())
		{
			return;
		}

		// TODO: Print all the data in DxilSignature.
		os << comment << "\n"
			<< comment << " " << name << " signature:\n"
			<< comment << "\n"
			<< comment << " Name                 Index             InterpMode\n"
			<< comment << " -------------------- ----- ----------------------\n";

		for (auto& sig_elt : sig_elts)
		{
			os << comment << " ";

			os << std::left << std::setw(20) << sig_elt->GetName();
			os << std::right << std::setw(6) << sig_elt->GetSemanticIndexVec()[0];
			os << std::right << std::setw(23) << sig_elt->GetInterpolationMode()->GetName();
			os << "\n";
		}
	}

	std::string GetTypeAndName(Type* ty, DxilFieldAnnotation& annotation, uint32_t array_size)
	{
		std::ostringstream oss;

		while (ty->IsArrayType())
		{
			ty = ty->ArrayElementType();
		}

		auto comp_ty_name = annotation.GetCompType().GetHLSLName();
		if (annotation.HasMatrixAnnotation())
		{
			auto const & matrix = annotation.GetMatrixAnnotation();
			switch (matrix.orientation)
			{
			case MatrixOrientation::RowMajor:
				oss << "row_major ";
				break;

			case MatrixOrientation::ColumnMajor:
				oss << "column_major ";
				break;

			default:
				DILITHIUM_UNREACHABLE("Wrong matrix orientation");
				break;
			}
			oss << comp_ty_name << matrix.rows << "x" << matrix.cols;
		}
		else if (ty->IsVectorType())
		{
			oss << comp_ty_name << ty->VectorNumElements();
		}
		else
		{
			oss << comp_ty_name;
		}

		oss << " " << annotation.GetFieldName();
		if (array_size)
		{
			oss << "[" << array_size << "]";
		}
		oss << ";";

		return oss.str();
	}

	void PrintStructLayout(StructType* st, DxilTypeSystem& type_sys,
		std::ostream& os, char const * comment,
		char const * var_name, uint32_t offset, uint32_t indent, uint32_t offset_indent, uint64_t size_of_struct = 0);

	void PrintFieldLayout(Type* ty, DxilFieldAnnotation& annotation, DxilTypeSystem& type_sys, std::ostream& os,
		char const * comment, uint32_t offset,
		uint32_t indent, uint32_t offset_indent, uint64_t size_to_print = 0)
	{
		offset += annotation.GetCBufferOffset();
		if (ty->IsStructType() && !annotation.HasMatrixAnnotation())
		{
			PrintStructLayout(cast<StructType>(ty), type_sys, os, comment, annotation.GetFieldName().c_str(), offset,
				indent, offset_indent);
		}
		else
		{
			auto elt_ty = ty;
			uint32_t array_size = 0;
			uint32_t array_level = 0;
			if (!HLMatrixLower::IsMatrixType(elt_ty) && elt_ty->IsArrayType())
			{
				array_size = 1;
				while (!HLMatrixLower::IsMatrixType(elt_ty) && elt_ty->IsArrayType())
				{
					array_size *= static_cast<uint32_t>(elt_ty->ArrayNumElements());
					elt_ty = elt_ty->ArrayElementType();
					++ array_level;
				}
			}

			if (annotation.HasMatrixAnnotation())
			{
				auto const & matrix = annotation.GetMatrixAnnotation();
				switch (matrix.orientation)
				{
				case MatrixOrientation::RowMajor:
					array_size /= matrix.rows;
					break;

				case MatrixOrientation::ColumnMajor:
					array_size /= matrix.cols;
					break;

				default:
					DILITHIUM_UNREACHABLE("Wrong matrix orientation");
					break;
				}
				if (elt_ty->IsVectorType())
				{
					elt_ty = elt_ty->VectorElementType();
				}
				else if (elt_ty->IsStructType())
				{
					unsigned col, row;
					elt_ty = HLMatrixLower::GetMatrixInfo(elt_ty, col, row);
				}
				if (array_level == 1)
				{
					array_size = 0;
				}
			}

			if (!HLMatrixLower::IsMatrixType(elt_ty) && elt_ty->IsStructType())
			{
				std::ostringstream oss;
				oss << annotation.GetFieldName();
				if (array_size)
				{
					oss << "[" << array_size << "]";
				}
				oss << ";";

				PrintStructLayout(cast<StructType>(elt_ty), type_sys, os, comment, oss.str().c_str(), offset,
					indent, offset_indent);
			}
			else
			{
				os << comment << std::string(indent, ' ');
				os << std::left << std::setw(offset_indent) << GetTypeAndName(ty, annotation, array_size);

				os << comment << " Offset:" << std::right << std::setw(5) << offset;
				if (size_to_print)
				{
					os << " Size: " << std::right << std::setw(5) << size_to_print;
				}
				os << "\n";
			}
		}
	}

	void PrintStructLayout(StructType* st, DxilTypeSystem& type_sys,
		std::ostream& os, char const * comment,
		char const * var_name, uint32_t offset, uint32_t indent, uint32_t offset_indent, uint64_t size_of_struct)
	{
		auto annotation = type_sys.GetStructAnnotation(st);
		os << comment << std::string(indent, ' ') << "struct " << st->Name() << "\n";
		os << comment << std::string(indent, ' ') << "{\n";
		os << comment << "\n";

		for (uint32_t i = 0; i < st->NumElements(); ++ i)
		{
			PrintFieldLayout(st->ElementType(i), annotation->FieldAnnotation(i), type_sys, os, comment,
				offset, offset_indent, offset_indent - 4);
		}
		os << comment << std::string(indent, ' ') << "\n";
		// The 2 in offsetIndent-indent-2 is for "} ".
		os << comment << std::string(indent, ' ') << "} " << std::left << std::setw(offset_indent - 2) << var_name;
		os << comment << " Offset:" << std::right << std::setw(5) << offset;
		if (size_of_struct)
		{
			os << " Size: " << std::right << std::setw(5) << size_of_struct;
		}
		os << "\n";

		os << comment << "\n";
	}

	void PrintCBufferDefinition(DxilCBuffer* buff, DxilTypeSystem& type_sys, std::ostream& os, char const * comment)
	{
		const unsigned offsetIndent = 50;
		auto gv = buff->GetGlobalSymbol();
		auto ty = gv->GetType()->PointerElementType();
		// For ConstantBuffer<> buf[2], the array size is in Resource binding count part.
		if (ty->IsArrayType())
		{
			ty = ty->ArrayElementType();
		}

		auto annotation = type_sys.GetStructAnnotation(cast<StructType>(ty));
		os << comment << " cbuffer " << buff->GetGlobalName() << "\n";
		os << comment << " {\n";
		os << comment << "\n";
		if (nullptr == annotation)
		{
			os << comment << "   [" << buff->GetSize() << " x i8] (type annotation not present)\n";
			os << comment << "\n";
		}
		else
		{
			PrintStructLayout(cast<StructType>(ty), type_sys, os, comment,
				buff->GetGlobalName().c_str(), /*offset*/ 0, /*indent*/ 3,
				offsetIndent, buff->GetSize());
		}
		os << comment << " }\n";
		os << comment << "\n";
	}

	void PrintTBufferDefinition(DxilResource* buff, DxilTypeSystem& type_sys, std::ostream& os, char const * comment)
	{
		uint32_t const offset_indent = 50;

		auto gv = buff->GetGlobalSymbol();
		auto ty = gv->GetType()->PointerElementType();
		// For TextureBuffer<> buf[2], the array size is in Resource binding count part.
		if (ty->IsArrayType())
		{
			ty = ty->ArrayElementType();
		}

		auto annotation = type_sys.GetStructAnnotation(cast<StructType>(ty));
		os << comment << " tbuffer " << buff->GetGlobalName() << "\n";
		os << comment << " {\n";
		os << comment << "\n";
		if (nullptr == annotation)
		{
			os << comment << "   (type annotation not present)\n";
			os << comment << "\n";
		}
		else
		{
			PrintStructLayout(cast<StructType>(ty), type_sys, os, comment,
				buff->GetGlobalName().c_str(), /*offset*/ 0, /*indent*/ 3,
				offset_indent, annotation->CBufferSize());
		}
		os << comment << " }\n";
		os << comment << "\n";
	}

	void PrintStructBufferDefinition(DxilResource* buff, DxilTypeSystem& type_sys, DataLayout const & dl,
		std::ostream& os, char const * comment)
	{
		uint32_t const offset_indent = 50;

		os << comment << " Resource bind info for " << buff->GetGlobalName() << "\n";
		os << comment << " {\n";
		os << comment << "\n";
		auto ret_ty = buff->GetRetType();
		// Skip none struct type.
		if (!ret_ty->IsStructType() || HLMatrixLower::IsMatrixType(ret_ty))
		{
			auto gv = buff->GetGlobalSymbol();
			auto ty = gv->GetType()->PointerElementType();
			// For resource array, use element type.
			if (ty->IsArrayType())
			{
				ty = ty->ArrayElementType();
			}
			// Get the struct buffer type like this %class.StructuredBuffer = type {
			// %struct.mat }.
			auto st = cast<StructType>(ty);
			auto annotation = type_sys.GetStructAnnotation(st);
			if (nullptr == annotation)
			{
				os << comment << "   [" << dl.TypeAllocSize(st) << " x i8] (type annotation not present)\n";
			}
			else
			{
				auto& field_annotation = annotation->FieldAnnotation(0);
				field_annotation.SetFieldName("$Element");
				PrintFieldLayout(ret_ty, field_annotation, type_sys, os,
					comment, /*offset*/ 0, /*indent*/ 3, offset_indent,
					dl.TypeAllocSize(st));
			}
			os << comment << "\n";
		}
		else
		{
			auto st = cast<StructType>(ret_ty);

			// TODO: struct buffer has different layout.
			// Cannot use cbuffer layout here.
			auto annotation = type_sys.GetStructAnnotation(st);
			if (nullptr == annotation)
			{
				os << comment << "   [" << dl.TypeAllocSize(st) << " x i8] (type annotation not present)\n";
			}
			else
			{
				PrintStructLayout(st, type_sys, os, comment, "$Element;",
					/*offset*/ 0, /*indent*/ 3, offset_indent, dl.TypeAllocSize(st));
			}
		}
		os << comment << " }\n";
		os << comment << "\n";
	}

	void PrintBufferDefinitions(DxilModule& mod, std::ostream& os, char const * comment)
	{
		os << comment << "\n"
			<< comment << " Buffer Definitions:\n"
			<< comment << "\n";
		auto& type_sys = mod.GetTypeSystem();

		for (auto& cbuf : mod.GetCBuffers())
		{
			PrintCBufferDefinition(cbuf.get(), type_sys, os, comment);
		}
		auto const & layout = mod.GetModule()->GetDataLayout();
		for (auto& res : mod.GetSRVs())
		{
			if (res->IsStructuredBuffer())
			{
				PrintStructBufferDefinition(res.get(), type_sys, layout, os, comment);
			}
			else if (res->IsTBuffer())
			{
				PrintTBufferDefinition(res.get(), type_sys, os, comment);
			}
		}
		for (auto& res : mod.GetUAVs())
		{
			if (res->IsStructuredBuffer())
			{
				PrintStructBufferDefinition(res.get(), type_sys, layout, os, comment);
			}
		}
	}

	void PrintResourceFormat(DxilResourceBase& res, uint32_t alignment, std::ostream& os)
	{
		switch (res.GetClass())
		{
		case ResourceClass::CBuffer:
		case ResourceClass::Sampler:
			os << std::right << std::setw(alignment) << "NA";
			break;

		case ResourceClass::UAV:
		case ResourceClass::SRV:
			switch (res.GetKind())
			{
			case ResourceKind::RawBuffer:
				os << std::right << std::setw(alignment) << "byte";
				break;

			case ResourceKind::StructuredBuffer:
				os << std::right << std::setw(alignment) << "struct";
				break;

			default:
				auto p = static_cast<DxilResource*>(&res);
				DxilCompType const & comp_type = p->GetCompType();
				char const * comp_name = comp_type.GetName();
				// TODO: add vector size.
				os << std::right << std::setw(alignment) << comp_name;
				break;
			}
		}
	}

	void PrintResourceDim(DxilResourceBase &res, unsigned alignment, std::ostream& os)
	{
		switch (res.GetClass())
		{
		case ResourceClass::CBuffer:
		case ResourceClass::Sampler:
			os << std::right << std::setw(alignment) << "NA";
			break;

		case ResourceClass::UAV:
		case ResourceClass::SRV:
			switch (res.GetKind())
			{
			case ResourceKind::RawBuffer:
			case ResourceKind::StructuredBuffer:
				if (res.GetClass() == ResourceClass::SRV)
				{
					os << std::right << std::setw(alignment) << "r/o";
				}
				else
				{
					auto& dxil_res = static_cast<DxilResource&>(res);
					if (!dxil_res.HasCounter())
					{
						os << std::right << std::setw(alignment) << "r/w";
					}
					else
					{
						os << std::right << std::setw(alignment) << "r/w+cnt";
					}
				}
				break;

			case ResourceKind::TypedBuffer:
				os << std::right << std::setw(alignment) << "buf";
				break;

			case ResourceKind::Texture2DMS:
			case ResourceKind::Texture2DMSArray:
				{
					auto& dxil_res = static_cast<DxilResource&>(res);
					std::string dim_name = res.GetResDimName();
					if (dxil_res.GetSampleCount())
					{
						dim_name += std::to_string(dxil_res.GetSampleCount());
					}
					os << std::right << std::setw(alignment) << dim_name;
				}
				break;

			default:
				os << std::right << std::setw(alignment) << res.GetResDimName();
				break;
			}
			break;
		}
	}

	void PrintResourceBinding(DxilResourceBase& res, std::ostream& os, char const * comment)
	{
		os << comment << " " << std::left << std::setw(31) << res.GetGlobalName();

		os << std::right << std::setw(10) << res.GetResClassName();

		PrintResourceFormat(res, 8, os);

		PrintResourceDim(res, 12, os);

		std::string ID = res.GetResIDPrefix();
		ID += std::to_string(res.GetID());
		os << std::right << std::setw(8) << ID;

		std::string bind = res.GetResBindPrefix();
		bind += std::to_string(res.GetLowerBound());
		if (res.GetSpaceID())
		{
			bind += ",space" + std::to_string(res.GetSpaceID());
		}

		os << std::right << std::setw(15) << bind;
		if (res.GetRangeSize() != UINT_MAX)
		{
			os << std::right << std::setw(6) << std::to_string(res.GetRangeSize()) << "\n";
		}
		else
		{
			os << std::right << std::setw(6) << "unbounded\n";
		}
	}

	void PrintResourceBindings(DxilModule& module, std::ostream& os, char const * comment)
	{
		os << comment << "\n"
			<< comment << " Resource Bindings:\n"
			<< comment << "\n"
			<< comment << " Name                                 Type  Format         Dim      ID      HLSL Bind  Count\n"
			<< comment << " ------------------------------ ---------- ------- ----------- ------- -------------- ------\n";

		for (auto& res : module.GetCBuffers())
		{
			PrintResourceBinding(*res, os, comment);
		}
		for (auto& res : module.GetSamplers())
		{
			PrintResourceBinding(*res, os, comment);
		}
		for (auto& res : module.GetSRVs())
		{
			PrintResourceBinding(*res, os, comment);
		}
		for (auto& res : module.GetUAVs())
		{
			PrintResourceBinding(*res, os, comment);
		}
		os << comment << "\n";
	}
}

void Usage()
{
	std::cerr << "Dilithium DirectX Intermediate Language Disassembler." << std::endl;
	std::cerr << "This program is free software, released under a MIT license" << std::endl;
	std::cerr << std::endl;
	std::cerr << "Usage: DilithiumDisasm INPUT [OUTPUT]" << std::endl;
	std::cerr << std::endl;
}

std::vector<uint8_t> LoadProgramFromStream(std::istream& in)
{
	in.seekg(0, std::ios_base::end);
	std::vector<uint8_t> program(in.tellg());
	in.seekg(0, std::ios_base::beg);
	in.read(reinterpret_cast<char*>(&program[0]), program.size());
	return program;
}

std::string Disassemble(std::vector<uint8_t> const & program)
{
	std::ostringstream oss;

	uint8_t const * il = program.data();
	uint32_t il_length = static_cast<uint32_t>(program.size());
	auto container = IsDxilContainerLike(il, il_length);
	if (container)
	{
		if (!IsValidDxilContainer(container, il_length))
		{
			TERROR("This container is invalid.");
		}

		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_FeatureInfo)
			{
				PrintFeatureInfo(reinterpret_cast<DxilShaderFeatureInfo const *>(GetDxilPartData(part)), oss, ";");
				break;
			}
		}
		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_InputSignature)
			{
				PrintSignature("Input", reinterpret_cast<DxilProgramSignature const *>(GetDxilPartData(part)), true, oss, ";");
				break;
			}
		}
		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_OutputSignature)
			{
				PrintSignature("Output", reinterpret_cast<DxilProgramSignature const *>(GetDxilPartData(part)), false, oss, ";");
				break;
			}
		}
		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_PatchConstantSignature)
			{
				PrintSignature("Patch Constant signature", reinterpret_cast<DxilProgramSignature const *>(GetDxilPartData(part)),
					false, oss, ";");
				break;
			}
		}

		uint32_t dxil_index = container->PartCount;
		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_DXIL)
			{
				dxil_index = i;
				break;
			}
		}

		if (dxil_index == container->PartCount)
		{
			TERROR("This container doesn't have DXIL.");
		}

		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_ShaderDebugInfoDXIL)
			{
				// Use dbg module if exist.
				dxil_index = i;
				break;
			}
		}

		auto dxil_part = GetDxilContainerPart(container, dxil_index);
		auto program_header = reinterpret_cast<DxilProgramHeader const *>(GetDxilPartData(dxil_part));
		if (!IsValidDxilProgramHeader(program_header, dxil_part->PartSize))
		{
			TERROR("The program header in this is container is invalid.");
		}

		for (uint32_t i = 0; i < container->PartCount; ++ i)
		{
			auto part = GetDxilContainerPart(container, i);
			if (part->PartFourCC == DFCC_PipelineStateValidation)
			{
				PrintPipelineStateValidationRuntimeInfo(GetDxilPartData(part), GetVersionShaderType(program_header->ProgramVersion),
					oss, ";");
				break;
			}
		}

		GetDxilProgramBitcode(program_header, &il, &il_length);
	}
	else
	{
		auto program_header = reinterpret_cast<DxilProgramHeader const *>(il);
		if (IsValidDxilProgramHeader(program_header, il_length))
		{
			GetDxilProgramBitcode(program_header, &il, &il_length);
		}
	}

	try
	{
		auto module = Dilithium::LoadLLVMModule(il, il_length, "");
		if (module->GetNamedMetadata("dx.version"))
		{
			auto& dxil_module = module->GetOrCreateDxilModule();
			PrintDxilSignature("Input", dxil_module.GetInputSignature(), oss, ";");
			PrintDxilSignature("Output", dxil_module.GetOutputSignature(), oss, ";");
			PrintDxilSignature("Patch Constant signature", dxil_module.GetPatchConstantSignature(), oss, ";");
			PrintBufferDefinitions(dxil_module, oss, ";");
			PrintResourceBindings(dxil_module, oss, ";");
		}

		DxcAssemblyAnnotationWriter w;
		module->Print(oss, &w);

		return oss.str();
	}
	catch (std::error_code& ec)
	{
		std::cerr << ec.message() << std::endl;
		return "";
	}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		Usage();
		return 1;
	}

	std::ifstream in(argv[1], std::ios_base::in | std::ios_base::binary);
	auto program = LoadProgramFromStream(in);
	in.close();

	auto text = Disassemble(program);

	std::ofstream out;
	bool screen_only = false;
	if (argc < 3)
	{
		screen_only = true;
	}
	else
	{
		out.open(argv[2]);
	}

	std::cout << text;
	if (!screen_only)
	{
		out << text;
	}
}
