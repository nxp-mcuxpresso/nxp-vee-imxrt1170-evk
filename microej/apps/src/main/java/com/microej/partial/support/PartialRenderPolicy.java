/*
 * Copyright 2020-2021 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.microej.partial.support;

import ej.microui.MicroUI;
import ej.microui.display.Display;
import ej.microui.display.GraphicsContext;
import ej.mwt.Container;
import ej.mwt.Desktop;
import ej.mwt.Widget;
import ej.mwt.render.RenderPolicy;
import ej.mwt.style.outline.NoOutline;

/**
 * This render policy is similar to {@link ej.mwt.render.OverlapRenderPolicy} but for platforms with partial buffer
 * support.
 * <p>
 * When using a partial buffer, its width is the same as the display but its height is smaller.
 * <p>
 * A partial buffer can only hold a number of lines of the display.
 * <p>
 * When using a partial buffer, a widget (/ widget tree) has to be rendered in multiple passes if its height is higher
 * than the height of the buffer. For each pass, a part of the widget is rendered in the partial buffer and the buffer
 * is flushed to the display.
 * <p>
 * Before calling {@link Display#flush()} the policy indicates to the platform where in the frame buffer should the
 * partial buffer be copied.
 * <p>
 * The platform has to implement the two natives defined in the <code>PartialBufferNatives</code> class.
 */
public class PartialRenderPolicy extends RenderPolicy {

	private final boolean supportOverlap;
	private final int bufferHeight;

	/**
	 * Creates a partial render policy.
	 *
	 * @param desktop
	 *            the desktop.
	 * @param supportOverlap
	 *            <code>true</code> to support overlapping widgets, <code>false</code> otherwise.
	 */
	public PartialRenderPolicy(Desktop desktop, boolean supportOverlap) {
		super(desktop);
		this.supportOverlap = supportOverlap;
		this.bufferHeight = PartialBufferNatives.getBufferHeight();
	}

	/**
	 * {@inheritDoc}
	 * <p>
	 * The graphics context is translated to the position of the parent of the widget. The clipping area is set to the
	 * intersection of the parent's bounds and the given bounds.
	 */
	@Override
	public void renderDesktop() {
		Desktop desktop = getDesktop();
		Widget widget = desktop.getWidget();
		if (widget != null) {
			renderWidget(widget, 0, 0, widget.getWidth(), widget.getHeight());
		}
	}

	@Override
	public void requestRender(Widget widget, int x, int y, int width, int height) {
		width = Math.min(width, widget.getWidth());
		height = Math.min(height, widget.getHeight());
		while (widget.isTransparent() || widget.getStyle().getMargin() != NoOutline.NO_OUTLINE) {
			Container parent = widget.getParent();
			if (parent == null) {
				break;
			}
			// Render the parent.
			x += parent.getContentX() + widget.getX();
			y += parent.getContentY() + widget.getY();
			widget = parent;
		}
		asynchronousRender(widget, x, y, width, height);
	}

	private void asynchronousRender(final Widget widget, final int x, final int y, final int width, final int height) {
		// Calling the call serially before increasing the pending repaints counter ensures that even if an
		// OutOfEventException occurs, the pendingRepaints remains consistent.
		MicroUI.callSerially(new Runnable() {
			@Override
			public void run() {
				executeRender(widget, x, y, width, height);
			}
		});
	}

	private void executeRender(Widget widget, int x, int y, int width, int height) {
		if (!widget.isAttached()) {
			return;
		}

		renderWidget(widget, x, y, width, height);
	}

	private void renderWidget(Widget widget, int x, int y, int width, int height) {
		int bufferHeight = this.bufferHeight;
		Desktop desktop = getDesktop();

		// reset translation and clip
		Display display = Display.getDisplay();
		GraphicsContext g = display.getGraphicsContext();
		g.resetTranslation();
		g.resetClip();
		g.translate(-widget.getAbsoluteX() - x, -widget.getAbsoluteY() - y);

		// compute translation and clip
		setParentClip(widget, g);
		g.intersectClip(widget.getX() + x, widget.getY() + y, width, height);

		int originTranslateX = g.getTranslationX();
		int originTranslateY = g.getTranslationY();
		int originClipX = g.getClipX();
		int originClipY = g.getClipY();
		int originClipWidth = g.getClipWidth();
		int originClipHeight = g.getClipHeight();

		int currentTranslateY = 0;
		int currentClipY = originClipY;
		int currentClipHeight = originClipHeight;

		while (currentClipHeight > 0) {
			int linesToRender = Math.min(bufferHeight, currentClipHeight);

			g.translate(0, -currentTranslateY);
			g.intersectClip(originClipX, currentClipY, originClipWidth, linesToRender);

			int translateX = g.getTranslationX();
			int translateY = g.getTranslationY();
			int clipX = g.getClipX();
			int clipY = g.getClipY();
			int flushX = widget.getAbsoluteX() + x;
			int flushY = widget.getAbsoluteY() + currentTranslateY + y;
			int flushWidth = g.getClipWidth();
			int flushHeight = g.getClipHeight();

			// render widget
			desktop.renderWidget(g, widget);

			// render its overlapping siblings
			if (this.supportOverlap) {
				g.setTranslation(translateX, translateY);
				g.setClip(clipX, clipY, flushWidth, flushHeight);
				paintSiblings(g, widget);
			}

			PartialBufferNatives.setFlushLimits(flushX, flushY, flushWidth, flushHeight);
			display.flush();

			g.setTranslation(originTranslateX, originTranslateY);
			g.resetClip();
			g.setClip(originClipX, originClipY, originClipWidth, originClipHeight);

			currentClipY += linesToRender;
			currentClipHeight -= linesToRender;
			currentTranslateY += linesToRender;
		}
	}

	private void paintSiblings(GraphicsContext g, Widget widget) {
		Container parent = widget.getParent();
		if (parent != null) {
			int translateX = g.getTranslationX();
			int translateY = g.getTranslationY();
			int clipX = g.getClipX();
			int clipY = g.getClipY();
			int clipWidth = g.getClipWidth();
			int clipHeight = g.getClipHeight();

			int childIndex = parent.getChildIndex(widget);
			int length = parent.getChildrenCount();
			for (int i = childIndex + 1; i < length; i++) {
				Widget child = parent.getChild(i);
				assert child != null;
				getDesktop().renderWidget(g, child);

				g.setTranslation(translateX, translateY);
				g.setClip(clipX, clipY, clipWidth, clipHeight);
			}
			g.translate(-parent.getX(), -parent.getY());
			paintSiblings(g, parent);
		}
	}

	private void setParentClip(Widget widget, GraphicsContext g) {
		Container parent = widget.getParent();
		if (parent != null) {
			setParentClip(parent, g);
			g.translate(parent.getX() + parent.getContentX(), parent.getY() + parent.getContentY());
			g.intersectClip(0, 0, parent.getContentWidth(), parent.getContentHeight());
		}
	}
}
