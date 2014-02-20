/*
 * Copyright (C) 2013 Tobias Lorenz.
 * Contact: tobias.lorenz@gmx.net
 *
 * This file is part of Tobias Lorenz's Toolkit.
 *
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * GNU General Public License 3.0 Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 */

/**
 * @mainpage
 *
 * <h1>Copyright</h1>
 * Copyright (C) 2013 Tobias Lorenz.<br>
 * Contact: <a href="mailto:tobias.lorenz@gmx.net">tobias.lorenz@gmx.net</a>
 *
 * <h1>Commercial License Usage</h1>
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Tobias Lorenz.
 *
 * <h1>GNU General Public License 3.0 Usage</h1>
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 * <h1>Data Model</h1>
 * @dot
 * digraph G {
 *   "Attribute" [ URL="@ref Vector::DBC::Attribute" ];
 *   "AttributeDefinition" [ URL="@ref Vector::DBC::AttributeDefinition" ];
 *   "AttributeRelation" [ URL="@ref Vector::DBC::AttributeRelation" ];
 *   "BitTiming" [ URL="@ref Vector::DBC::BitTiming" ];
 *   "Database" [ URL="@ref Vector::DBC::Database", style=bold ];
 *   "EnvironmentVariable" [ URL="@ref Vector::DBC::EnvironmentVariable", style=bold ];
 *   "ExtendedMultiplexor" [ URL="@ref Vector::DBC::ExtendedMultiplexor" ];
 *   "Message" [ URL="@ref Vector::DBC::Message", style=bold ];
 *   "Node" [ URL="@ref Vector::DBC::Node", style=bold ];
 *   "Signal" [ URL="@ref Vector::DBC::Signal", style=bold ];
 *   "SignalGroup" [ URL="@ref Vector::DBC::SignalGroup" ];
 *   "SignalType" [ URL="@ref Vector::DBC::SignalType" ];
 *   "ValueDescriptions" [ URL="@ref Vector::DBC::ValueDescriptions" ];
 *   "ValueTable" [ URL="@ref Vector::DBC::ValueTable" ];
 *
 *   "Database" -> "BitTiming" [ label="bitTiming" ];
 *   "Database" -> "Node" [ label="nodes" ];
 *   "Database" -> "ValueTable" [ label="valueTables" ];
 *   "Database" -> "Message" [ label="messages" ];
 *   "Database" -> "EnvironmentVariable" [ label="environmentVariables" ];
 *   "Database" -> "SignalType" [ label="signalTypes" ];
 *   "Database" -> "AttributeDefinition" [ label="attributeDefinitions" ];
 *   "Database" -> "Attribute" [ label="attributeDefaults" ];
 *   "Database" -> "Attribute" [ label="attributeValues" ];
 *   "Database" -> "AttributeRelation" [ label="attributeRelationValues" ];
 *
 *   "EnvironmentVariable" -> "Node" [ label="accessNodes", style=dotted ];
 *   "EnvironmentVariable" -> "ValueDescriptions" [ label="valueDescriptions" ];
 *   "EnvironmentVariable" -> "Attribute" [ label="attributeValues" ];
 *
 *   "ExtendedMultiplexor" -> "Signal" [ label="switchName", style=dotted ];
 *
 *   "Message" -> "Node" [ label="transmitter", style=dotted ];
 *   "Message" -> "Signal" [ label="signals" ];
 *   "Message" -> "Node" [ label="transmitters", style=dotted ];
 *   "Message" -> "SignalGroup" [ label="signalGroups" ];
 *   "Message" -> "Attribute" [ label="attributeValues" ];
 *
 *   "Node" -> "Attribute" [ label="attributeValues" ];
 *
 *   "AttributeRelation" -> "Attribute" [ label="base/derived class" ];
 *   "AttributeRelation" -> "Node" [ label="nodeName", style=dotted ];
 *   "AttributeRelation" -> "EnvironmentVariable" [ label="environmentVariableName", style=dotted ];
 *   "AttributeRelation" -> "Message" [ label="messageId", style=dotted ];
 *   "AttributeRelation" -> "Signal" [ label="signalName", style=dotted ];
 *
 *   "Signal" -> "Node" [ label="receivers", style=dotted ];
 *   "Signal" -> "ValueDescriptions" [ label="valueDescriptions" ];
 *   "Signal" -> "SignalType" [ label="type", style=dotted ];
 *   "Signal" -> "Attribute" [ label="attributeValues" ];
 *   "Signal" -> "ExtendedMultiplexor" [ label="extendedMultiplexors" ];
 *
 *   "SignalGroup" -> "Signal" [ label="signals", style=dotted ];
 *
 *   "SignalType" -> "ValueTable" [ label="valueTable", style=dotted ];
 *
 *   "ValueTable" -> "ValueDescriptions" [ label="valueDescriptions" ];
 * }
 * @enddot
 */
