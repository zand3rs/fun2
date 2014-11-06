/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.yuicon.fun2.ards;

//import com.javaexchange.dbConnectionBroker.DbConnectionBroker;
import com.yuicon.utils.db.SimpleDBConnection;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.ResultSet;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author paolo
 */
public class ARDSRequest extends HttpServlet {

	private String dbDriver = "";
	private String dbServer = "";
	private String dbUser = "";
	private String dbPassword = "";
	private String dbName = "";
	private String dbPort = "";
	private String minConns = "5";
	private String maxConns = "25";
	private String dbLogFile = "";
	private SimpleDBConnection dbConnPool = null;
	private Connection conn = null;
	private PreparedStatement query = null;
	private int defaultValue = 0;
	private long node = 1;
	private long  Id = 0;
	private long msisdn = -1;
	private long ref_id = -1;
	private long keyword_id = -1;
	private ResultSet rs = null;
	private String imsi = null;
	private String temp = null;
	private int operation = -1;
	private int rna = -1;
	public ARDSRequest() {
	}

	/**
	 * Processes requests for both HTTP <code>GET</code> and <code>POST</code> methods.
	 * @param request servlet request
	 * @param response servlet response
	 * @throws ServletException if a servlet-specific error occurs
	 * @throws IOException if an I/O error occurs
	 */
	protected void processRequest(HttpServletRequest request, HttpServletResponse response)
		throws ServletException, IOException {
			response.setContentType("text/html;charset=UTF-8");
			PrintWriter out = response.getWriter();
			dbUser     = "";
			dbPassword = "";
			dbName     = "";
			dbDriver = "";
			dbServer = "";
			minConns = "5";
			maxConns = "25";
			dbLogFile = "";

			imsi = null;
			rna = -1;
			operation = -1; 
			temp = null;
			temp = "";
			try {
				imsi = request.getParameter("IMSI");
				if (imsi.length() < 1) {
					getServletContext().log("one of the required parameters (IMSI) is null");
					response.sendError(response.SC_BAD_REQUEST, "Bad request" +
							" (invalid format or parameters)");
					out.close();
					return;	
				}
				/*if ((imsi.compareTo("515021808853425") != 0) && (imsi.compareTo("515021808853428") !=0) && (imsi.compareTo("515021808853427") !=0 ) && (imsi.compareTo("515021808853426") != 0) && (imsi.compareTo("515021808853429") !=0) && (imsi.compareTo("515022107590559") != 0) && (imsi.compareTo("515022107590560") !=0) && (imsi.compareTo("515022107590561") != 0) && (imsi.compareTo("515020405565113") != 0) && (imsi.compareTo("515020405565114") != 0) && (imsi.compareTo("515020405565115") != 0) && (imsi.compareTo("515021309395868") != 0) && ( imsi.compareTo("515021309395869") !=0) && (imsi.compareTo("515021309395870") !=0)) {
					getServletContext().log("DEV MODE ON INVALID IMSI:" + imsi + ". IMSI not defined in whitelisted test list.");
					out.close(); 
					return;
				}*/
			} catch (Exception e) {
				getServletContext().log("one of the required parameters (IMSI) is null");
				response.sendError(response.SC_BAD_REQUEST, "Bad request" +
						" (invalid format or parameters)");
				out.close();
				return;	
			}
			try {
				temp = request.getParameter("type");
				if (temp.length() > 0) {
					operation = Integer.parseInt(temp);
				} else {
					getServletContext().log("one of the required parameters (type) is null");
					response.sendError(response.SC_BAD_REQUEST, "Bad request" +
							" (invalid format or parameters)");
					out.close();
					return;
				}
			} catch (Exception e) {
				getServletContext().log("one of the required parameters (type) is null");
				response.sendError(response.SC_BAD_REQUEST, "Bad request" +
						" (invalid format or parameters)");
				out.close();
				return;
			}

			temp = null;
			temp = "";
			try {
				temp = request.getParameter("rna");
				if (temp.length() > 0) {
					rna = Integer.parseInt(temp);
				}
			} catch (Exception e) {
				getServletContext().log("No RNA Parameter for  transaction with IMSI:" + imsi + " and type:" + operation);
			}

			getServletContext().log("IMSI:" + imsi + " TYPE:" + operation +
					" RNA:" + rna);
			if (operation != 1) {
				getServletContext().log("Deactivation Not Yet Allowed in FUN2 CPK");
				out.close();
				return;
			}
			conn = null;
			try { 
				conn = dbConnPool.getConnection();
			} catch (Exception e) {
				if (conn == null) {
					this.dbUser = getServletConfig().getInitParameter("dbUsername");
					this.dbPassword = getServletConfig().getInitParameter("dbPassword");
					getServletContext().log("Username/Password: " + dbUser + "/" + dbPassword);
					if (null == dbUser || null == dbPassword) {
						getServletContext().log("Null login detail found!");
					}

					dbServer = getServletConfig().getInitParameter("dbHost");
					dbPort = getServletConfig().getInitParameter("dbPort");
					dbName = getServletConfig().getInitParameter("dbName");

					dbLogFile = getServletConfig().getInitParameter("dbLog");
					minConns = getServletConfig().getInitParameter("dbPoolMin");
					maxConns = getServletConfig().getInitParameter("dbPoolMax");

					if (null == dbUser || null == dbPassword || null == dbServer || null == dbPort
							|| null == dbName || null == dbLogFile || minConns == null ||  maxConns == null) {
						getServletContext().log("ARDSRequest Servlet Null database detail found!");
					} else {
						try {
							int port = Integer.parseInt(dbPort);
							int min  = Integer.parseInt(minConns);
							int max  = Integer.parseInt(maxConns);

							dbConnPool = new SimpleDBConnection("jdbc:oracle:thin:@" + dbServer,"oracle.jdbc.driver.OracleDriver",dbUser,dbPassword, min,max, "SELECT SYSDATE FROM DUAL");
							conn = dbConnPool.getConnection();
						}
						catch (Exception ex) {
							getServletContext().log("ARDSRequest Servlet Encountered generic exception!" + ex.getMessage());
							// Internal Server Error SC_INTERNAL_SERVER_ERROR 
							response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
									" (Error in connecting to database)");
							out.close();
							return;
						}
					}
				}
			}
			rs = null;
			int select = 0;
			if (rna == 1) {	
				getServletContext().log("FIRST RNA OPERATION FOR IMSI:" + imsi + " TYPE:" + operation + " RNA:" + rna);
				try {
					// CHECK IF ROAMING COMMAND WAS SENT 
					query = conn.prepareStatement("SELECT REF_ID,MSISDN,IMSI,ID FROM keywordRequest where IMSI=? and status=0");
					query.setString(1,imsi);
					rs = query.executeQuery();
				} catch (Exception e){
					getServletContext().log("ARDSRequest Servlet Exception " + e.getMessage());
					response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
							" (Error in connecting to database)");
					try {
						conn.close();
					} catch(Exception e2) {
					}
					query = null;
					out.close();
					return;
				}

				try {
					ref_id = -1;
					msisdn = -1;
					keyword_id = -1;
					//imsi = "";
					while (rs.next()) {
						ref_id = rs.getLong(1);
						msisdn = rs.getLong(2);
						//imsi = rs.getString(3);
						keyword_id = rs.getLong(4);
						select = 1;
						break;
					}
					rs.close();
					rs = null;
					query.close();
					query = null;
				} catch (Exception ex) {
					getServletContext().log("ARDSRequest Servlet SQL Exception " + ex.getMessage());
					response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
							" (Error in connecting to database)");
					out.close();
					return;
				}
				if (select == 1) {
					// USER SENT ROAM ON
					getServletContext().log("USER PREVIOUSLY SENT ROAM ON IMSI:" + imsi + " TYPE:" + operation + " RNA:" + rna + " ref_id:" + ref_id);
					try {
						query = conn.prepareStatement("update request_log set status=4 where id=? and status=3");
						query.setLong(1, ref_id);
						query.executeUpdate();
						conn.commit();
						query.close();
						query = null;
						query = conn.prepareStatement("update keywordRequest set status=2,end_tm=SYSDATE where id=? and status=0");
						query.setLong(1, keyword_id);
						query.executeUpdate();
						conn.commit();
						query.close();
						query = null;
					} catch (Exception ex) {
						getServletContext().log("ARDSRequest Servlet SQL Exception " + ex.getMessage());
						response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
								" (Error in connecting to database)");
						out.close();
						return;
					} finally {
						//dbPool.freeConnection(conn);
						try {
							conn.close();
						} catch (Exception e) {
						}
					}
				} else {
					// USER DID NOT SEND ROAM ON AUTOPROVISION FOR ROAMING 
					getServletContext().log("USER AUTOPROVISION ON IMSI:" + imsi + " TYPE:" + operation + " RNA:" + rna);
					select = 0;
					while (select == 0) {
						try {
							// generate/assign transaction_id
							query = conn.prepareStatement("SELECT MANUAL_ENROLLMENT_LOG_SEQ.nextval from dual");
							rs = query.executeQuery();
							select = 1;
						} catch (Exception e){
							getServletContext().log("ARDSRequest Servlet Exception " + e.getMessage());
							response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
									" (Error in connecting to database)");
							//dbPool.freeConnection(conn);
							try {
								conn.close();
							} catch(Exception e2) {
							}
							out.close();
							return;
						}
					}

					try {
						while (rs.next()) {
							Id = rs.getLong(1);
							break;
						}
						rs.close();
						rs = null;
						query.close();
						query = null;
					} catch (Exception ex) {
						getServletContext().log("ARDSRequest Servlet SQL Exception " + ex.getMessage());
						response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
								" (Error in connecting to database)");
						out.close();
						return;
					} finally {
						//dbPool.freeConnection(conn);
					}



					try {
						query = conn.prepareStatement("insert into manual_enrollment_log(id, MSISDN, IMSI,ENROLLMENT_TYPE,STATUS,DT_CREATED,REQUEST_ORIGIN,SILENT,BULK_UPLOAD,TX_DATE,MIN_BAL) VALUES (?,?,?,?,?,SYSDATE,?,?,?,trunc(SYSDATE),100)");


						query.setLong(1, Id);
						query.setLong(2, -1);
						query.setString(3, imsi);
						if (operation == 1) {
							query.setString(4, "ACTIVATION");				
						} else {
							query.setString(4, "DEACTIVATION");
						}
						query.setInt(5, 0);
						query.setString(6, "ARDS");
						if (rna == -1) {
							query.setInt(7, 1);
						} else {
							query.setInt(7, 0);
						}
						query.setLong(8, 0);
						query.executeUpdate();
						conn.commit();
						query.close();
						query = null;
					} catch (Exception ex) {
						getServletContext().log("ARDSRequest Servlet SQL Exception " + ex.getMessage());
						response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
								" (Error in connecting to database)");
						out.close();
						return;
					} finally {
						//dbPool.freeConnection(conn);
						try {
							conn.close();
						} catch (Exception e) {
						}
					}
				}
			} else {
				getServletContext().log("CHANGE LOCATION FOR IMSI:" + imsi + " TYPE:" + operation + " RNA:" + rna);
				select = 0;
				String status = "";
				String reason = "";
				long tempStatus=0;	
				long tempCode=0;	
				if (operation == 1) {
					try {
						// generate/assign transaction_id
						query = conn.prepareStatement("SELECT STATUS,DEACTIVATION_REASON from sim_activation where IMSI=?");
						query.setString(1,imsi);
						rs = query.executeQuery();
						while (rs.next()) {
							status = rs.getString(1);
							reason = rs.getString(2);
							break;
						}
						rs.close();
						rs = null;
						query.close();
						query = null;
						if (status.length() > 0) {
							if (status.compareTo("INACTIVE") == 0) {
								if ((reason.compareTo("GROAM NO") == 0) || (reason.compareTo("GROAM OFF") == 0)) {
									getServletContext().log("CHANGE LOCATION FOR IMSI:" + imsi + " TYPE:" + operation + " RNA:" + rna + " IGNORED. User Current INACTIVE because user previously sent:" + reason);
									try {
										conn.close();
									} catch(Exception e2) {
									}
									out.close();
									return;
								} else {
									// Check if previous activation failed because of insuff bal
									query = conn.prepareStatement("SELECT * from (SELECT status,error_code from request_log where tran_type=8 and IMSI=? order by id desc) where rownum =1");
									query.setString(1,imsi);
									rs = query.executeQuery();
									while (rs.next()) {
										tempStatus = rs.getLong(1);
										tempCode = rs.getLong(2);
										break;
									}
									rs.close();
									rs = null;
									query.close();
									query = null;
									if ((tempStatus == -1) && (tempCode == 108)) {
										getServletContext().log("CHANGE LOCATION FOR IMSI:" + imsi + " TYPE:" + operation + " RNA:" + rna + " Previous First RNA failed because of insufficient Balance issues. Resimulating First RNA");
										rna = 1;
									}
								} 
							}
						}
					} catch (Exception e){
						getServletContext().log("ARDSRequest Servlet Exception " + e.getMessage());
						response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
								" (Error in connecting to database)");
						//dbPool.freeConnection(conn);
						try {
							conn.close();
						} catch(Exception e2) {
						}
						out.close();
						return;
					}
				}
				select = 0;
				while (select == 0) {
					try {
						// generate/assign transaction_id
						query = conn.prepareStatement("SELECT MANUAL_ENROLLMENT_LOG_SEQ.nextval from dual");
						rs = query.executeQuery();
						select = 1;
					} catch (Exception e){
						getServletContext().log("ARDSRequest Servlet Exception " + e.getMessage());
						response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
								" (Error in connecting to database)");
						//dbPool.freeConnection(conn);
						try {
							conn.close();
						} catch(Exception e2) {
						}
						out.close();
						return;
					}
				}

				try {
					while (rs.next()) {
						Id = rs.getLong(1);
						break;
					}
					rs.close();
					rs = null;
					query.close();
					query = null;
				} catch (Exception ex) {
					getServletContext().log("ARDSRequest Servlet SQL Exception " + ex.getMessage());
					response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
							" (Error in connecting to database)");
					out.close();
					return;
				}

				try {
					//getServletContext().log("This is the rangelist " + rangeList); 
					query = conn.prepareStatement("insert into manual_enrollment_log(id, MSISDN, IMSI,ENROLLMENT_TYPE,STATUS,DT_CREATED,REQUEST_ORIGIN,SILENT,BULK_UPLOAD,TX_DATE,MIN_BAL) VALUES (?,?,?,?,?,SYSDATE,?,?,?,SYSDATE,100)");


					query.setLong(1, Id);
					query.setLong(2, -1);
					query.setString(3, imsi);
					if (operation == 1) {
						query.setString(4, "ACTIVATION");				
					} else {
						query.setString(4, "DEACTIVATION");
					}
					query.setInt(5, 0);
					query.setString(6, "ARDS");
					if (rna == -1) {
						query.setInt(7, 1);
					} else {
						query.setInt(7, 0);
					}
					query.setLong(8, 0);
					query.executeUpdate();
					conn.commit();
					query.close();
					query = null;
				} catch (Exception ex) {
					getServletContext().log("ARDSRequest Servlet SQL Exception " + ex.getMessage());
					response.sendError(response.SC_INTERNAL_SERVER_ERROR, "Internal Server Error" +
							" (Error in connecting to database)");
					out.close();
					return;
				} finally {
					try {
						conn.close();
					} catch (Exception e) {
					}
				}
			}
		}

	// <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
	/**
	 * Handles the HTTP <code>GET</code> method.
	 * @param request servlet request
	 * @param response servlet response
	 * @throws ServletException if a servlet-specific error occurs
	 * @throws IOException if an I/O error occurs
	 */
		protected void doGet(HttpServletRequest request, HttpServletResponse response)
		throws ServletException, IOException {
			processRequest(request, response);
		}

	/**
	 * Handles the HTTP <code>POST</code> method.
	 * @param request servlet request
	 * @param response servlet response
	 * @throws ServletException if a servlet-specific error occurs
	 * @throws IOException if an I/O error occurs
	 */
		protected void doPost(HttpServletRequest request, HttpServletResponse response)
		throws ServletException, IOException {
			processRequest(request, response);
		}

	/**
	 * Returns a short description of the servlet.
	 * @return a String containing servlet description
	 */
		public String getServletInfo() {
			return "Short description";
		}// </editor-fold>

	public void init() {
		//rangeList = getInitParameter("number_range");
		dbUser     = "";
		dbPassword = "";
		dbName     = "";
		dbDriver = "";
		dbServer = "";
		minConns = "5";
		maxConns = "25";
		dbLogFile = "";
		//dbPool = null;
		dbConnPool = null;
		String start = "";
		String end = "";

		this.dbUser = getInitParameter("dbUsername");
		this.dbPassword = getInitParameter("dbPassword");
		getServletContext().log("Username/Password: " + dbUser + "/" + dbPassword);
		if (null == dbUser || null == dbPassword) {
			getServletContext().log("Null login detail found!");
		}

		dbServer = getInitParameter("dbHost");
		dbPort = getInitParameter("dbPort");
		dbName = getInitParameter("dbName");

		getServletContext().log("Login/Database: " + dbUser + "/" + dbPassword + " " + dbServer + ":" + dbPort + "/" + dbPassword);

		dbLogFile = getInitParameter("dbLog");
		minConns = getInitParameter("dbPoolMin");
		maxConns = getInitParameter("dbPoolMax");
		if (null == dbUser || null == dbPassword || null == dbServer || null == dbPort
				|| null == dbName || null == dbLogFile || minConns == null ||  maxConns == null) {
			getServletContext().log("Null database detail found!");
		} else {
			try {
				int port = Integer.parseInt(dbPort);
				int min  = Integer.parseInt(minConns);
				int max  = Integer.parseInt(maxConns);

				//dbPool = new DbConnectionBroker("oracle.jdbc.driver.OracleDriver", "jdbc:oracle:thin:@" + dbServer + ":" + dbPort+ ":" + dbName ,dbUser, dbPassword, min, max, dbLogFile, 48, false, 0, 0);
				//dbPool = new DbConnectionBroker("oracle.jdbc.driver.OracleDriver", "jdbc:oracle:thin:@" + dbServer ,dbUser, dbPassword, min, max, dbLogFile, 0, false, 0, 0);
				dbConnPool = new SimpleDBConnection("jdbc:oracle:thin:@" + dbServer,"oracle.jdbc.driver.OracleDriver",dbUser,dbPassword, min,max, "SELECT SYSDATE FROM DUAL");
			}
			catch (NumberFormatException ex) {
				getServletContext().log("Non-integer database detail found!", ex);
			}
			catch (Exception ex) {
				getServletContext().log("Encountered generic exception!", ex);
			}
		}
	}
}
