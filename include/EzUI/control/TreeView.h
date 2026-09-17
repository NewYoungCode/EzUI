#pragma once
#include "HListView.h"
#include "VListView.h"
#include "VLayout.h"
#include "HLayout.h"
#include "Label.h"

namespace ezui {
	class NodeLabel :public HLayout {
	public:
		Label m_btn;
		Label m_text;
	public:
		NodeLabel(Object* owner = NULL);
		virtual ~NodeLabel();
		virtual void OnLayout()EZUI_OVERRIDE;
		void SetText(const UIString& text);
		UIString GetText();
		void Expand();
		void Collapse();
	};
};
namespace ezui {
	class TreeView;

	//树形菜单节点
	class EZUI_API TreeNode :public VListView {
		friend class TreeView;
	private:
		//显示节点名称
		Control* m_nodeLabel;
		//节点显示的文字
		UIString m_text;
		//节点显示的图标
		Image* m_icon;
		//子节点面板
		VListView* m_childrenPanel;
	private:
		//内部使用 当调用此节点被使用的时候或者调用了GetText SetText SetIcon的时候才会调用此函数检查创建节点头部
		void CreateNode();
		virtual Control* AddChild(Control* ctrl, bool takeOwnership = false)EZUI_OVERRIDE {
			auto* node = ctrl->As<TreeNode>();
			EZUI_ASSERT(node, L"TreeView child must be a TreeNode");
			return (Control*)AddNode(node, takeOwnership);
		};
		using VListView::RemoveChild;
		using VListView::RemoveAll;
		using VListView::InsertChildAt;
		using VListView::SwapChild;
	protected:
		//正在创建节点的时候(重写此函数来达到diy效果 返回的控件请正确设置绝对宽高,返回的控件请自行管理释放)
		virtual Control* OnCreateNode(const UIString& nodeText, Image* icon);
		//当用户调用GetText函数的时候会进入此函数
		virtual UIString OnGetText();
		//当用户调用SetText函数的时候会进入此函数
		virtual void OnSetText(const UIString& nodeText);
		//当用户调用SetIcon函数的时候会进入此函数
		virtual void OnSetIcon(Image* icon);
	public:
		TreeNode(Object* ownerObject = NULL);

		/// 添加子节点；takeOwnership 为 true 时同时取得对象所有权。
		TreeNode* AddNode(TreeNode* childNode, bool takeOwnership = false);

		/// 插入子节点；takeOwnership 为 true 时同时取得对象所有权。
		TreeNode* InsertNode(int pos, TreeNode* childNode, bool takeOwnership = false);

		/// 移除子节点；deleteNode 为 true 时同时立即删除该节点。
		bool RemoveNode(TreeNode* childNode, bool deleteNode = false);

		/// 移除全部子节点；deleteNodes 为 true 时同时立即删除这些节点。
		void RemoveAllNodes(bool deleteNodes = false);

		virtual void SetAttribute(const UIString& attrName, const UIString& attrValue)EZUI_OVERRIDE;

		//获取节点的显示名称
		UIString GetText();

		//设置节点的名称
		void SetText(const UIString& nodeText);

		//设置节点的名称
		void SetIcon(Image* icon);

		//获取子节点
		std::vector<TreeNode*> GetChildNodes();

		//获取顶层的TreeView对象
		TreeView* GetTreeView();

		//展开节点
		void Expand();

		//收起节点
		void Collapse();

		//节点是否已展开
		bool IsExpanded();

		virtual ~TreeNode();
	};
};

namespace ezui {
	//树形列表控件
	class EZUI_API TreeView :public Control {
	private:
		//垂直滚动条
		VScrollBar* m_vScrollBar;
		//水平滚动条
		HScrollBar* m_hScrollBar;
	private:
		void Init();
		using Control::RemoveChild;
		using Control::RemoveAll;
		virtual Control* AddChild(Control* ctrl, bool takeOwnership = false)EZUI_OVERRIDE {
			auto* node = ctrl->As<TreeNode>();
			EZUI_ASSERT(node, L"TreeNode child must be a TreeNode");
			return (Control*)AddNode(node, takeOwnership);
		};
	protected:
		virtual void OnLayout()EZUI_OVERRIDE;
		virtual void OnScroll(int offsetX, int offsetY)EZUI_OVERRIDE;
		void OnChildPaint(PaintEventArgs* args)EZUI_OVERRIDE;
	public:
		TreeView(Object* ownerObject = NULL);
		/// 添加根节点；takeOwnership 为 true 时同时取得对象所有权。
		TreeNode* AddNode(TreeNode* rootNode, bool takeOwnership = false);
		/// 插入根节点；takeOwnership 为 true 时同时取得对象所有权。
		TreeNode* InsertNode(int pos, TreeNode* rootNode, bool takeOwnership = false);
		/// 移除根节点；deleteNode 为 true 时同时立即删除该节点。
		bool RemoveNode(TreeNode* childNode, bool deleteNode = false);
		/// 移除全部根节点；deleteNodes 为 true 时同时立即删除这些节点。
		void RemoveAllNodes(bool deleteNodes = false);
		//获取根节点
		std::vector<TreeNode*> GetRootNodes();
		virtual VScrollBar* GetVScrollBar()EZUI_OVERRIDE;
		virtual HScrollBar* GetHScrollBar()EZUI_OVERRIDE;
		virtual ~TreeView();
	};
};
