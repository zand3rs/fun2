/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
File: SimpleDBConnection.java
Description: Simple database connection using DBCP.
Created by: Mark Raymund Tejero
Date: 09-12-2008

Solutions Engineering Center
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Date        Author                                    Revision
 02-19-2009  Onin Brinas                               Modified class to implement proper DBCP
                                                       connection pooling using BasicDataSource.
                                                       Added javadocs.
 06-19-2009  Onin Brinas                               Removed dbPoolStats method. Moved functiona-
                                                       lity to stat handler.
 07-06-2009  Onin Brinas                               Removed debug variable and method in favor
                                                       of calling logger.* functions.
 07-07-2009  Onin Brinas                               Improved logging.
 07-28-2009  Onin Brinas                               Removed leading and trailing space from the
                                                       code.
 07-31-2009  Onin Brinas                               Documented settings.
 08-04-2009  Onin Brinas                               Code updates based on inputs taken from class
                                                       diagram.
 08-18-2009  Onin Brinas                               Updated codes based on source code analysis.
 09-01-2009  Onin Brinas                               Added stacktrace logging on level fatal/error
                                                       exceptions.
 04-30-2010  Onin Brinas                               Removed constructor that has no
                                                       validationQuery.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
package com.yuicon.utils.db;

// JDK imports
import java.sql.*;

// DBCP imports
import org.apache.commons.dbcp.BasicDataSource;

// Log4j imports
import org.apache.log4j.Logger;
import spiffy.log4j.LoggerHelper;

/**
 * Simple database connection using DBCP.
 */
public class SimpleDBConnection extends BasicDataSource
{
    /**
     * Name used to identify this class' entries in the log.
     */
    private String classTrackingName = "";
    /**
     * Logger variable.
     */
    protected static final Logger logger = LoggerHelper.getLogger();
    /**
     * The default time between eviction runs. Default is every 4 minutes.
     */
    public static final long TIME_BETWEEN_EVICTION_RUNS_DEFAULT = 240000L;
    /** Determinues if there is an sqlConnection */
    public boolean sqlConn = true; //worker, selector
    
    /**
     * Default constructor for preparing a SimpleDBConnection with connection
     * testing using default TIME_BETWEEN_EVICTION_RUNS_DEFAULT.
     * @param url the connection uri. e.g. "jdbc:MYSQL://localhost/dbname".
     * @param driverClassName e.g. "com.mysql.jdbc.Driver".
     * @param username the username for the DB connection.
     * @param password the password for the DB connection.
     * @param minIdle the minimum number of DB connections to be reserved.
     * @param maxIdle the minimum number of DB connections that can be used.
     * @param validationQuery validation query to use.
     */
    public SimpleDBConnection(String url, String driverClassName, String username, String password, int minIdle, int maxIdle, String validationQuery)
    {
        this(url, driverClassName, username, password, minIdle, maxIdle, validationQuery, TIME_BETWEEN_EVICTION_RUNS_DEFAULT);
    }

    /**
     * Default constructor for preparing a SimpleDBConnection with connection
     * testing using default TIME_BETWEEN_EVICTION_RUNS_DEFAULT.
     * @param url the connection uri. e.g. "jdbc:MYSQL://localhost/dbname".
     * @param driverClassName e.g. "com.mysql.jdbc.Driver".
     * @param username the username for the DB connection.
     * @param password the password for the DB connection.
     * @param minIdle the minimum number of DB connections to be reserved.
     * @param maxIdle the minimum number of DB connections that can be used.
     * @param validationQuery validation query to use.
     * @param sqlConn determines if ans sql connection is present
    */
    public SimpleDBConnection(String url, String driverClassName, String username, String password, int minIdle, int maxIdle, String validationQuery, boolean sqlConn)
    {
        this(url, driverClassName, username, password, minIdle, maxIdle, validationQuery, TIME_BETWEEN_EVICTION_RUNS_DEFAULT);
        this.sqlConn = sqlConn;
    }
    
    /**
     * Default constructor for preparing a SimpleDBConnection with connection
     * testing.
     * @param url the connection uri. e.g. "jdbc:MYSQL://localhost/dbname".
     * @param driverClassName e.g. "com.mysql.jdbc.Driver".
     * @param username the username for the DB connection.
     * @param password the password for the DB connection.
     * @param minIdle the minimum number of DB connections to be reserved.
     * @param maxIdle the minimum number of DB connections that can be used.
     * @param validationQuery validation query to use.
     * @param timeBetweenEvictionRunsMillis the time between eviction runs.
     */
    public SimpleDBConnection(String url, String driverClassName, String username, String password, int minIdle, int maxIdle, String validationQuery, long timeBetweenEvictionRunsMillis)
    {
        classTrackingName = this.getClass().getSimpleName();

        // Set defaults
        setUrl(url);
        setDriverClassName(driverClassName); // "com.mysql.jdbc.Driver"
        setUsername(username);
        setPassword(password);
        setMinIdle(minIdle);
        setMaxIdle(maxIdle);
        setValidationQuery(validationQuery);
        setTimeBetweenEvictionRunsMillis(timeBetweenEvictionRunsMillis);
        setMinEvictableIdleTimeMillis(timeBetweenEvictionRunsMillis * 2); // Replace idle connections that existed for more than 2 runs of the idle object eviction thread.
    }

    /**
     * Gets the value of the classTrackingName variable.
     * @return value of classTrackingName.
     */
    public String getClassTrackingName()
    {
        return classTrackingName;
    }

    /**
     * Initializes the database's connection eviction thread.
     * @return true on success, false otherwise
     */
    public boolean initialize()
    {
        String methodName = "initialize";

        logger.info(getClassTrackingName() + ": " + methodName + ". Start loading driver=>" + this.driverClassName + " url=>" + this.url);

        setTestOnBorrow(true); // Reactive measure for stale connections. Checks connections before they are given to the borrower.
        setTestWhileIdle(true); // Preventive measure for stale connections. Runs the idle object eviction thread to checks idle connections.
        int testsPerEviction = minIdle;
        if (testsPerEviction <= 0)
        {
            testsPerEviction = 1;
        }
        setNumTestsPerEvictionRun(testsPerEviction); // Number of connections that the idle object eviction thread will check per run.

        logger.info(getClassTrackingName() + ": " + methodName + ". DB settings. minIdle=>" + getMinIdle() +
                                                                          " maxIdle=>" + getMaxIdle() +
                                                                          " validationQuery=>" + getValidationQuery() +
                                                                          " testOnBorrow=>" + getTestOnBorrow() +
                                                                          " testWhenIdle=>" + getTestWhileIdle() +
                                                                          " timeBetweenEvictionRunsMillis=>" + getTimeBetweenEvictionRunsMillis() +
                                                                          " minEvictableIdleTimeMillis=>" + getMinEvictableIdleTimeMillis() +
                                                                          " numTestsPerEvictionRun=>" + getNumTestsPerEvictionRun());

        logger.info(getClassTrackingName() + ": " + methodName + ". Done  loading driver=>" + this.driverClassName);

        return true;
    }

    /**
     * Gets a connection from the connection pool.
     * @return database connection
     */
    public synchronized Connection dbPoolGetConnection()
    {
        String methodName = "dbPoolGetConnection";
        try
        {
            sqlConn = true;
            return getConnection();
        }
        catch (SQLException sqle)
        {
            sqlConn = false;
            logger.error(getClassTrackingName() + ": " + methodName + ". SQLException: " + sqle.getMessage(), sqle);
            return null;
        }
        catch (Exception e)
        {
            sqlConn = false;
            logger.error(getClassTrackingName() + ": " + methodName + ". Exception: " + e.getMessage(), e);
            return null;
        }
    }

    /**
     * Closes the database connection and frees the pool.
     */
    public synchronized void dbShutdown()
    {
        String methodName = "dbShutdown";
        logger.info(getClassTrackingName() + ": " + methodName + ". Start disconnecting from DB");

        try
        {
            close();
            sqlConn = true;
        }
        catch (SQLException sqle)
        {
            sqlConn = false;
            // There is nothing we can do for this
            logger.error(getClassTrackingName() + ": " + methodName + ". SQLException: " + sqle.getMessage(), sqle);
        }
        catch (Exception e)
        {
            sqlConn = false;
            // There is nothing we can do for this
            logger.error(getClassTrackingName() + ": " + methodName + ". Exception: " + e.getMessage(), e);
        }

        logger.info(getClassTrackingName() + ": " + methodName + ". Done  disconnecting from DB");
    }
}
