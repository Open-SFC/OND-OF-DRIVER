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
 * File name: ValidateSchema.java
 * Author: Freescale Semiconductor
 * Date:   03/13/2013
 * Description: 
 * 
 */

package com.fsl.utils;

import java.io.File;
import java.io.IOException;

import javax.xml.XMLConstants;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Source;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class ValidateSchema {

	public boolean validateDocument(String xsd, String xml)
			throws ParserConfigurationException, SAXException, IOException {
		// parse an XML document into a DOM tree
		DocumentBuilder parser = DocumentBuilderFactory.newInstance()
				.newDocumentBuilder();
        File xmlFile = new File(xml);
		Document document = parser.parse(xmlFile);

        Node rootNode = getRootNode(document);
        NamedNodeMap attributes = rootNode.getAttributes();
        Node schemaVer = attributes.getNamedItem("schemaVersion");
        float xmlVer = 0;
        float xsdVer = 0;
        if(schemaVer != null)
        {
           String version = schemaVer.getNodeValue();
           xmlVer = Float.parseFloat(version);
           //System.out.println("schemaVer : " + xmlVer);
        }

        Document schemaDoc =  parser.parse(new File(xsd));
        Node root = getRootNode(schemaDoc);
        NamedNodeMap attribs = root.getAttributes();
        Node xsdVersion = attribs.getNamedItem("version");
        
        if(xsdVersion != null)
        {
           xsdVer = Float.parseFloat(xsdVersion.getNodeValue());
        }
         
        if(xmlVer > xsdVer)  
        {
           System.out.println("Unable to validate xml " + xmlFile.getName() 
               + ", its version " + xmlVer 
               + " is not compatible with  the schema version " + xsdVer);
           return false;
        }

		// create a SchemaFactory capable of understanding WXS schemas
		SchemaFactory factory = SchemaFactory
				.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);

		// load a WXS schema, represented by a Schema instance
		Source schemaFile = new StreamSource(new File(xsd));
		Schema schema = factory.newSchema(schemaFile);

		// create a Validator instance, which can be used to validate an
		// instance document
		Validator validator = schema.newValidator();

		// validate the DOM tree
		validator.validate(new DOMSource(document));
        return true;
	}

	public void validateXMLDOM(Document document, String xsd)
			throws ParserConfigurationException, SAXException, IOException {
	
		// create a SchemaFactory capable of understanding WXS schemas
		SchemaFactory factory = SchemaFactory
				.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);

		// load a WXS schema, represented by a Schema instance
		Source schemaFile = new StreamSource(new File(xsd));
		Schema schema = factory.newSchema(schemaFile);

		// create a Validator instance, which can be used to validate an
		// instance document
		Validator validator = schema.newValidator();

		// validate the DOM tree
		try {
			validator.validate(new DOMSource(document));
		} catch (SAXException e) {
			// instance document is invalid!
			e.printStackTrace();
		}

	}
        public static Node getRootNode(Document doc) {
          NodeList nodeList = doc.getChildNodes();
          int size = nodeList.getLength();
          for (int i = 0; i < size; i++) {
            Node node = nodeList.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE) {
                return node;
            }
          }
          return null;
       }

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		String xmlFile = args[0];
		String xsdFile = args[1];
		ValidateSchema valSche = new ValidateSchema();
		try {
			valSche.validateDocument(xsdFile, xmlFile);
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
