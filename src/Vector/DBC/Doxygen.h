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
 * <h1>Class relations</h1>
 * @dot
 * digraph G {
 *   "Attribute" [ URL="@ref Vector::DBC::Attribute" ];
 *   "AttributeDefinition" [ URL="@ref Vector::DBC::AttributeDefinition" ];
 *   "BitTiming" [ URL="@ref Vector::DBC::BitTiming" ];
 *   "Database" [ URL="@ref Vector::DBC::Database" ];
 *   "EnvironmentVariable" [ URL="@ref Vector::DBC::EnvironmentVariable" ];
 *   "ExtendedMultiplexor" [ URL="@ref Vector::DBC::ExtendedMultiplexor" ];
 *   "Message" [ URL="@ref Vector::DBC::Message" ];
 *   "Node" [ URL="@ref Vector::DBC::Node" ];
 *   "Signal" [ URL="@ref Vector::DBC::Signal" ];
 *   "SignalGroup" [ URL="@ref Vector::DBC::SignalGroup" ];
 *   "SignalType" [ URL="@ref Vector::DBC::SignalType" ];
 *   "ValueTable" [ URL="@ref Vector::DBC::ValueTable" ];
 *
 *   "Database" -> "BitTiming" [ label="bitTiming" ];
 *   "Database" -> "Node" [ label="nodes" ];
 *   "Database" -> "ValueTable" [ label = "valueTables" ];
 *   "Database" -> "Message" [ label = "messages" ];
 *   "Database" -> "EnvironmentVariable" [ label = "environmentVariables" ];
 *   "Database" -> "SignalType" [ label = "signalTypes" ];
 *   "Database" -> "AttributeDefinition" [ label = "attributeDefinitions" ];
 *   "Database" -> "Attribute" [ label = "attributeDefaults" ];
 *   "Database" -> "Attribute" [ label = "attributeValues" ];
 *
 *   "EnvironmentVariable" -> "Attribute" [ label = "attributeValues" ];
 *
 *   "Message" -> "Signal" [ label = "signals" ];
 *   "Message" -> "SignalGroup" [ label = "signalGroups" ];
 *   "Message" -> "Attribute" [ label = "attributeValues" ];
 *
 *   "Node" -> "Attribute" [ label = "attributeValues" ];
 *
 *   "Signal" -> "Attribute" [ label = "attributeValues" ];
 *   "Signal" -> "ExtendedMultiplexor" [ label = "extendedMultiplexors" ];
 * }
 * @enddot
 */
