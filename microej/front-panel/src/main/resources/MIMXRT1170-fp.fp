<?xml version="1.0"?>
<!--
	Front Panel

	Copyright 2022 MicroEJ Corp. All rights reserved.
	This library is provided in source code for use, modification and test, subject to license terms.
	Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
-->
<frontpanel
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xmlns="http://xml.is2t.com/ns/1.0/frontpanel"
	xsi:schemaLocation="http://xml.is2t.com/ns/1.0/frontpanel .fp1.0.xsd">

	<device name="MIMXRT1170" skin="MIMXRT1170-fp_background.jpg">
		<ej.fp.widget.Display x="63" y="91" width="720" height="1280" refreshStrategyClass="ej.fp.widget.display.brs.PredrawRefreshStrategy" bufferPolicyClass="ej.fp.widget.display.buffer.SwapTripleBufferPolicy"/>
		<ej.fp.widget.Pointer x="63" y="91" width="720" height="1280" touch="true"/>
	</device>
</frontpanel>
