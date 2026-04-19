#pragma once
#include "core/UIDef.h"
#include "GraphicsTypes.h"

namespace ezui {
	//几何图形基础类(支持自定义路径)
	class EZUI_API Geometry {
	protected:
		ID2D1GeometrySink* m_pSink = NULL;
		ID2D1Geometry* m_rgn = NULL;
		Geometry(const Geometry& rightCopy) = delete;
	public:
		Geometry();
		virtual ~Geometry();
		void AddArc(const PointF& endPoint, float radius);
		void AddAcr(const D2D1_ARC_SEGMENT& arc);
		void AddLine(const PointF& endPoint);
		void BeginFigure(const PointF& startPoint, D2D1_FIGURE_BEGIN figureBegin = D2D1_FIGURE_BEGIN_FILLED);
		void CloseFigure(D2D1_FIGURE_END figureEnd = D2D1_FIGURE_END_CLOSED);
		ID2D1Geometry* Get()const;
	public:
		/// <summary>
		/// 将两个几何图形通过指定的合并模式（Union、Intersect、Xor、Exclude）合并到一个输出几何中。
		/// </summary>
		/// <param name="out">合并结果输出到该 Geometry。</param>
		/// <param name="a">参与合并的第一个 Geometry。</param>
		/// <param name="b">参与合并的第二个 Geometry。</param>
		/// <param name="COMBINE_MODE">几何合并模式，取值如 D2D1_COMBINE_MODE_UNION、INTERSECT、XOR、EXCLUDE。</param>
		static void Combine(Geometry& out, const Geometry& a, const Geometry& b, D2D1_COMBINE_MODE COMBINE_MODE);

		/// <summary>
		/// 合并两个区域，取它们的联合部分（即最大边界区域）。
		/// </summary>
		static void Union(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_UNION);
		}

		/// <summary>
		/// 获取两个区域的交集部分。
		/// </summary>
		static void Intersect(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_INTERSECT);
		}

		/// <summary>
		/// 合并两个区域，保留不重叠的部分（异或运算）。
		/// </summary>
		static void Xor(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_XOR);
		}

		/// <summary>
		/// 从第一个区域中排除第二个区域的部分（差集）。
		/// </summary>
		static void Exclude(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_EXCLUDE);
		}
	};

	//矩形(已经完成闭合)
	class EZUI_API RectangleGeometry :public Geometry {
	private:
		void Create(float x, float y, float width, float height, float _radius);
	public:
		RectangleGeometry(float x, float y, float width, float height, float _radius = 0);
		RectangleGeometry(const RectF& _rect, float radius = 0);
		RectangleGeometry(const RectF& _rect, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius);
		virtual ~RectangleGeometry() {};
	};

	//扇形(已经完成闭合)
	class EZUI_API PieGeometry :public Geometry {
	public:
		PieGeometry(const RectF& rectF, float startAngle, float endAngle);
		virtual ~PieGeometry() {};
	};

	//圆形/椭圆(已经完成闭合)
	class EZUI_API EllipseGeometry :public PieGeometry {
	public:
		EllipseGeometry(const RectF& rectF) :PieGeometry(rectF, 0, 360) {}
		virtual ~EllipseGeometry() {};
	};

	//贝塞尔曲线
	struct Bezier {
	public:
		PointF point1;
		PointF point2;
		PointF point3;
	};
};
