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
 * File name: Generate.java
 * Author: Freescale Semiconductor
 * Date:   03/13/2013
 * Description: This file is used to generate the output file.
 * 
 */

package com.fsl.utils;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.StringTokenizer;

public class Generate {
	/**
	 * @param xmlInURI
	 * @param xslInURI
	 * @param fileName
	 */
	void writeContents(String xmlInURI, String xslInURI, String fileName) 
														throws Exception {
		FileWriter fileWriter = null;
		BufferedWriter buffWriter = null;
		try {
			File outputFile = new File(fileName);
			if (!outputFile.exists()) {
				outputFile.createNewFile();
			}
			fileWriter = new FileWriter(outputFile, true);
			buffWriter = new BufferedWriter(fileWriter);

			String outputString = XMLUtils.transform(xmlInURI, xslInURI);

			if (outputString.indexOf("typedef") != -1) {
				generateID(fileName, outputString);
			} else if (outputString.indexOf("#define CM_DM") != -1) {
				generateMacros(fileName, outputString);
			} else if ((outputString.indexOf("REFSTR") != -1)
					|| (outputString.indexOf("REFNAME") != -1)) {
				String outRefFile = XMLReader.OUT_DIR + XMLReader.REF_STRS_H;
				generateStructure(outRefFile, outputString, buffWriter);
			} else {
				buffWriter.write(outputString);
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (Exception e) {
			//e.printStackTrace();
			throw new Exception(e.getMessage());
		} finally {
			try {
				buffWriter.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	private void generateStructure(String fileName, String outputString,
			BufferedWriter buffWriter) throws Exception {
		StringTokenizer strTok = new StringTokenizer(outputString, "|");

		while (strTok.hasMoreTokens()) {

			String token = strTok.nextToken();
			if ((null != token)
					&& ((token.indexOf("REFSTR") != -1) || (token
							.indexOf("REFNAME") != -1))) {
				// write to another header file
				FileWriter filWriter = null;
				BufferedWriter bufWriter = null;
				try {
					File outFile = new File(fileName);

					if (!outFile.exists()) {
						outFile.createNewFile();
						filWriter = new FileWriter(outFile, true);
						bufWriter = new BufferedWriter(filWriter);

						bufWriter.write(XMLReader.HEAD_STR);
						bufWriter.write("\n");
						bufWriter.write("#ifndef CM_SECAPPLREF_H");
						bufWriter.write("\n");
						bufWriter.write("#define CM_SECAPPLREF_H");
						bufWriter.write("\n");
						bufWriter.write("\n");
						bufWriter.write(token);
						bufWriter.write("#endif");
						bufWriter.flush();
						bufWriter.close();
					} else {
						String[] tokArr = token.split(":");
						for (int i = 0; i < tokArr.length; i++) {
							if (tokArr[i].indexOf("*") == -1) {
								appendToFileConditional(fileName,
										tokArr[i].trim(), token);
							}
						} // for
					}
				} catch (Exception e) {
					e.printStackTrace();
					throw new Exception(e.getMessage());
				}
			} else {
				buffWriter.write(token);
			}
		} // while
	} // generateStructure
	
	/**
	 * @param fileName
	 * @param outputString
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public void writeToFile(String fileName, String outputString)
			throws FileNotFoundException, IOException {
		RandomAccessFile randomAccessFile = null;
		try {
			randomAccessFile = new RandomAccessFile(fileName, "rw");
			if(randomAccessFile.readLine() == null) {
				randomAccessFile.writeBytes(outputString);
			} else {
				long len = randomAccessFile.length();
				randomAccessFile.seek(len);
				randomAccessFile.writeBytes(outputString);
			}
			
			randomAccessFile.close();
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(-1);
		}finally {
			if(null != randomAccessFile) {
				try {
					randomAccessFile.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}
	private void appendToFileConditional(String fileName, String str,
			String outputString) throws IOException {
		RandomAccessFile randomAccessFile = new RandomAccessFile(fileName, "rw");
		String readLine = null;
		boolean alreadyExist = false;
		while ((readLine = randomAccessFile.readLine()) != null) {
			if(readLine.indexOf(str) != -1) {
				alreadyExist = true;
				break;
			}else if (readLine.trim().equals("#endif")) {
				long filePointer = randomAccessFile.getFilePointer();
				int length = (readLine).getBytes().length;
				randomAccessFile.seek(filePointer - length - 1);
				randomAccessFile.writeBytes("\n");
				randomAccessFile.writeBytes(outputString);
			}
		} // while
		if(alreadyExist == false) {
			randomAccessFile.writeBytes("#endif");
		}
		randomAccessFile.close();
	}

	/**
	 * @param fileName
	 * @param outputString
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	private void generateMacros(String fileName, String outputString)
			throws FileNotFoundException, IOException {
		RandomAccessFile randomAccessFile = new RandomAccessFile(fileName, "rw");
		if (randomAccessFile.readLine() == null) {
			randomAccessFile.writeBytes("#ifndef CM_SECAPPL_H");
			randomAccessFile.writeBytes("\n");
			randomAccessFile.writeBytes("#define CM_SECAPPL_H");
			randomAccessFile.writeBytes("\n");
			randomAccessFile.writeBytes(outputString);
		} else {
			String readLine = null;
			while ((readLine = randomAccessFile.readLine()) != null) {
				if (readLine.equals("#endif")) {
					long filePointer = randomAccessFile.getFilePointer();
					int length = (readLine).getBytes().length;
					randomAccessFile.seek(filePointer - length - 1);
					randomAccessFile.writeBytes("\n");
					randomAccessFile.writeBytes(outputString);
				}
			}
		}
		randomAccessFile.writeBytes("#endif");
	}

	/**
	 * @param fileName
	 * @param outputString
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	private void generateID(String fileName, String outputString)
			throws FileNotFoundException, IOException {
		RandomAccessFile randomAccessFile = new RandomAccessFile(fileName, "rw");
		randomAccessFile.writeBytes("#ifndef CM_SECAPPL_H");
		randomAccessFile.writeBytes("\n");
		randomAccessFile.writeBytes("#define CM_SECAPPL_H");
		randomAccessFile.writeBytes("\n");
		if (randomAccessFile.readLine() == null) {
			String nodeID = "CM_DM_ROOT_NODE_ID = 1,";
			boolean flag = false;
			StringTokenizer tk = new StringTokenizer(outputString, "\n");
			while (tk.hasMoreTokens()) {
				String token = tk.nextToken();
				if (!(token.equals("}DMNodeID_e;"))) {
					randomAccessFile.writeBytes(token);
					randomAccessFile.writeBytes("\n");
				} else if (token.equals("}DMNodeID_e;")) {
					randomAccessFile.writeBytes(token);
				}

				if (!flag) {
					randomAccessFile.writeBytes(nodeID);
					randomAccessFile.writeBytes("\n");
					flag = true;
				}
			}
		} else {
			String readLine = null;
			StringTokenizer tk = new StringTokenizer(outputString);

			while ((readLine = randomAccessFile.readLine()) != null) {
				if (readLine.equals("}DMNodeID_e;")) {

					long filePointer = randomAccessFile.getFilePointer();
					int length = readLine.getBytes().length;
					randomAccessFile.seek((filePointer - 1) - length);
					while (tk.hasMoreTokens()) {
						String token = tk.nextToken();
						if (token.endsWith("ID,")) {
							randomAccessFile.writeBytes(token);
							randomAccessFile.writeBytes("\n");
						}
					}
					break;
				}
			}
			randomAccessFile.writeBytes("}DMNodeID_e;");
			randomAccessFile.writeBytes("\n");
			randomAccessFile.writeBytes("#endif");
			randomAccessFile.writeBytes("\n\n");
		}
	}

	/**
	 * @param args
	 */
	public static void main(String args[]) {
		Generate generate = new Generate();
		try {
			if (args.length == 0 || args.length < 3) {

				System.out
						.println("*******************************************");
				System.out
						.println("USAGE: "
								+ "XMLUtils XML_FileName, XSL_FileName, OutputFileName");
				System.out
						.println("USAGE:"
								+ "java XMLUtils c:/IPDB_DM.xml c:/comp.xsl c:/dmtest.c");
				System.out
						.println("*******************************************");
				return;
			}

			String xmlFile = args[0];
			String xslFile = args[1];
			String outputFile = args[2];

			generate.writeContents(xmlFile.trim(), xslFile.trim(), outputFile);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
