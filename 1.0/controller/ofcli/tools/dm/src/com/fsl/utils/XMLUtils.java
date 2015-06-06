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
 * File name: XMLUtils.java
 * Author: Freescale Semiconductor
 * Date:   03/13/2013
 * Description: This file is used to parse the XML file.
 * 
 */

package com.fsl.utils;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Result;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.w3c.dom.Document;
import org.xml.sax.SAXException;

public class XMLUtils {

	public static String compareTokens(String instPath, String path) {
            String retVal = "yes";
            String pathStr = "";
            String instStr = "";
            if(instPath.equals(path) || instPath.indexOf(path) == -1) {
               return "no";
            }
            String[] instToks = instPath.split("\\.");
            String[] pathToks = path.split("\\.");
	    //System.out.println("instToks.length= " + instToks.length);
	    //System.out.println("pathToks.length= " + pathToks.length);
            
            for (int i = 0; i < pathToks.length; i++) {
              instStr = instToks[i];
              pathStr = pathToks[i];
              if(pathStr.trim().indexOf("{0}") != -1) {
                 pathStr = pathStr.trim().substring(0, pathStr.indexOf("{0}"));
              }
              if(instStr.trim().indexOf("{0}") != -1) {
                 instStr = instStr.trim().substring(0, instStr.indexOf("{0}"));
              }
              if(!pathStr.equals(instStr)) {
                retVal = "no";
                break;
              }
            }
            return retVal;
	}
	public static void throwError(String param) throws Exception{
		throw new Exception(param);
	}
    // For XSLT debug prints
	public static void debugMsg(String param) {
		System.out.println(param);
	}
	public static String transform(String xmlInURI, String xslInURI) throws Exception {
		String retValue = null;
		try {
			TransformerFactory tFactory = TransformerFactory.newInstance();
			Templates translet = tFactory.newTemplates(new StreamSource(
					xslInURI));
			Transformer transformer = translet.newTransformer();
			DocumentBuilderFactory docFact = DocumentBuilderFactory
					.newInstance();
			DocumentBuilder docBuild = docFact.newDocumentBuilder();
			Document doc = docBuild.parse(xmlInURI);
			ByteArrayOutputStream bos = new ByteArrayOutputStream();
			Result result = new StreamResult(bos);
			transformer.transform(new DOMSource(doc), result);
			retValue = bos.toString("UTF-8");
		} catch (TransformerConfigurationException tce) {
			throw new TransformerConfigurationException(tce);
		} catch (ParserConfigurationException pce) {
			throw new ParserConfigurationException();
		} catch (IOException ioe) {
			throw new IOException(ioe);
		} catch (TransformerException te) {
			throw new TransformerException(te);
		} catch (SAXException se) {
			throw new SAXException(se);
		}
		return retValue;
	}// transform
}
