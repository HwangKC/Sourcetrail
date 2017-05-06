#ifndef QT_GRAPH_VIEW_H
#define QT_GRAPH_VIEW_H

#include <set>

#include <QGraphicsView>
#include <QPointF>

#include "component/view/GraphView.h"
#include "data/graph/Graph.h"
#include "qt/utility/QtScrollSpeedChangeListener.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/types.h"

struct DummyEdge;
struct DummyNode;
class MessageActivateTrail;
class QLabel;
class QMouseEvent;
class QPushButton;
class QSequentialAnimationGroup;
class QSlider;
class QtGraphEdge;
class QtGraphicsView;
class QtGraphNode;

class QtGraphView
	: public QObject
	, public GraphView
{
	Q_OBJECT

public:
	QtGraphView(ViewLayout* viewLayout);
	virtual ~QtGraphView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	virtual void rebuildGraph(
		std::shared_ptr<Graph> graph,
		const std::vector<std::shared_ptr<DummyNode>>& nodes,
		const std::vector<std::shared_ptr<DummyEdge>>& edges,
		const GraphParams params);
	virtual void clear();

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds(const std::vector<Id>& defocusedTokenIds);

	virtual void resizeView();

	virtual Vec2i getViewSize() const;

	virtual void scrollToValues(int xValue, int yValue);

	virtual void activateEdge(Id edgeId, bool centerOrigin);

private slots:
	void updateScrollBars();
	void finishedTransition();
	void clickedInEmptySpace();
	void pressedCharacterKey(QChar c);
	void scrolled(int);

	void trailDepthChanged(int);

	void clickedCollapse();
	void clickedExpand();

	void clickedBackwardTrail();
	void clickedForwardTrail();

private:
	MessageActivateTrail getMessageActivateTrail(bool forward);
	void activateTrail(bool forward);
	void updateTrailButtons();

	void switchToNewGraphData();

	QtGraphicsView* getView() const;

	void doRebuildGraph(
		std::shared_ptr<Graph> graph,
		const std::vector<std::shared_ptr<DummyNode>>& nodes,
		const std::vector<std::shared_ptr<DummyEdge>>& edges,
		const GraphParams params);
	void doClear();
	void doResize();
	void doRefreshView();

	void doFocusIn(const std::vector<Id>& tokenIds);
	void doFocusOut(const std::vector<Id>& tokenIds);

	std::shared_ptr<QtGraphNode> findNodeRecursive(const std::list<std::shared_ptr<QtGraphNode>>& nodes, Id tokenId);

	std::shared_ptr<QtGraphNode> createNodeRecursive(
		QGraphicsView* view, std::shared_ptr<QtGraphNode> parentNode, const DummyNode* node, bool multipleActive);
	std::shared_ptr<QtGraphEdge> createEdge(
		QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds, Graph::TrailMode trailMode, QPointF pathOffset);
	std::shared_ptr<QtGraphEdge> createAggregationEdge(
		QGraphicsView* view, const DummyEdge* edge, std::set<Id>* visibleEdgeIds);

	QRectF itemsBoundingRect(const std::list<std::shared_ptr<QtGraphNode>>& items) const;
	QRectF getSceneRect(const std::list<std::shared_ptr<QtGraphNode>>& items) const;

	void centerNode(QtGraphNode* node);

	void compareNodesRecursive(
		std::list<std::shared_ptr<QtGraphNode>> newSubNodes,
		std::list<std::shared_ptr<QtGraphNode>> oldSubNodes,
		std::list<QtGraphNode*>* appearingNodes,
		std::list<QtGraphNode*>* vanishingNodes,
		std::vector<std::pair<QtGraphNode*, QtGraphNode*>>* remainingNodes);

	void createTransition();

	QtThreadedFunctor<
		std::shared_ptr<Graph>,
		const std::vector<std::shared_ptr<DummyNode>>&,
		const std::vector<std::shared_ptr<DummyEdge>>&,
		const GraphParams
	> m_rebuildGraphFunctor;
	QtThreadedFunctor<void> m_clearFunctor;
	QtThreadedFunctor<void> m_resizeFunctor;
	QtThreadedFunctor<void> m_refreshFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_focusInFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_focusOutFunctor;
	QtThreadedLambdaFunctor m_onQtThread;

	std::shared_ptr<Graph> m_graph;
	std::shared_ptr<Graph> m_oldGraph;

	std::list<std::shared_ptr<QtGraphEdge>> m_edges;
	std::list<std::shared_ptr<QtGraphEdge>> m_oldEdges;

	std::list<std::shared_ptr<QtGraphNode>> m_nodes;
	std::list<std::shared_ptr<QtGraphNode>> m_oldNodes;

	std::vector<std::shared_ptr<QtGraphNode>> m_activeNodes;
	std::shared_ptr<QtGraphNode> m_oldActiveNode;

	bool m_centerActiveNode;
	bool m_scrollToTop;
	bool m_restoreScroll;
	Vec2i m_scrollValues;
	bool m_isIndexedList;

	std::shared_ptr<QSequentialAnimationGroup> m_transition;
	QPointF m_sceneRectOffset;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListenerHorizontal;
	QtScrollSpeedChangeListener m_scrollSpeedChangeListenerVertical;

	QWidget* m_trailWidget;
	QPushButton* m_expandButton;
	QPushButton* m_collapseButton;
	QPushButton* m_forwardTrailButton;
	QPushButton* m_backwardTrailButton;
	QSlider* m_trailDepthSlider;
	QLabel* m_trailDepthLabel;

	std::vector<QRectF> m_virtualNodeRects;
};

#endif // QT_GRAPH_VIEW_H
