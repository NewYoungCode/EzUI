#pragma once
#include "HListView.h"
#include "VListView.h"
#include "VLayout.h"
#include "HLayout.h"
#include "Label.h"

namespace ezui {
	class TreeView;

	//树形菜单节点
	class UI_EXPORT TreeNode :public VListView {
	private:
		//不设置节点头的时候就会显示节点名称
		Label& m_nodeName;
		//节点头部布局器(默认是水平布局器)
		Control* m_nodeHeader = NULL;
		//子节点集合
		VListView& m_childNodes;
	protected:
		//正在创建节点的时候(重写此函数来达到diy效果 返回的控件请正确设置绝对宽高)
		virtual Control* OnCreateNode(const UIString& nodeText, Image* icon = NULL);
	public:
		TreeNode(Object* ownerObject = NULL);
		TreeNode* AddNode(TreeNode* childNode);
		//获取节点的显示名称
		UIString GetText();
		//设置节点的名称
		virtual void SetText(const UIString& nodeText, Image* icon = NULL);
		//获取标题部分的区域 可以diy树形菜单的节点名称等...
		Control* GetHeader();
		//设置头部控件 用于diy节点名称 (设置进来的头部容器请指定宽高 以免不显示)
		void SetHeader(Control* header);
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
	//树形菜单
	class UI_EXPORT TreeView :public Control {
	private:
		//垂直滚动条
		VScrollBar* m_vScrollBar;
		//水平滚动条
		HScrollBar* m_hScrollBar;
	private:
		void Init();
		void Offset(int offsetX, int offsetY);
	protected:
		virtual void OnLayout()override;
		void OnChildPaint(PaintEventArgs& args)override;
	public:
		TreeView(Object* ownerObject = NULL);
		TreeNode* AddNode(TreeNode* rootNode);
		//获取根节点
		std::vector<TreeNode*> GetRootNodes();
		virtual VScrollBar* GetVScrollBar()override;
		virtual HScrollBar* GetHScrollBar()override;
		virtual ~TreeView();
	};
};