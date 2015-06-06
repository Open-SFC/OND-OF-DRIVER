/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: XMLNodeReplace.java
 * Author: Freescale Semiconductor
 * Date:   03/13/2013
 * Description: This file is used to updated the id.xml file with
 *  		largest NodeID.
 * 
 */

package com.fsl.utils;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

public class XMLNodeReplace {
	
	static void changenodevalue(String inputFile, String outputFile,
			String value) throws SAXException, IOException,
			ParserConfigurationException, XPathExpressionException,
			TransformerConfigurationException,
			TransformerFactoryConfigurationError, TransformerException {
		
		Document doc = DocumentBuilderFactory.newInstance()
				.newDocumentBuilder().parse(new InputSource(inputFile));

		// locate the node(s)
		XPath xpath = XPathFactory.newInstance().newXPath();
		NodeList nodes = (NodeList) xpath.evaluate("//root/nodeId", doc,
				XPathConstants.NODESET);

		// make the change
		for (int idx = 0; idx < nodes.getLength(); idx++) {
			nodes.item(idx).setTextContent(value);
		}

		// save the result
		Transformer xformer = TransformerFactory.newInstance().newTransformer();
		xformer.transform(new DOMSource(doc), new StreamResult(new File(
				outputFile)));
	}
}
