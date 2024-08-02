#include <iostream>
#include <string>
#include <mariadb/conncpp.hpp>
#include <vector>
#include<fstream>
#include<cmath>
class fiveteam
{           
public:
 
    fiveteam()
    {}
    ~fiveteam()
    {}
    
    void dustavg(std::unique_ptr<sql::Connection> &conn, std::string region)
        {
            std::string clock[24] = {"% 0:00", "% 1:00", "% 2:00", "% 3:00", "% 4:00", "% 5:00", "% 6:00", "% 7:00", "% 8:00", "% 9:00", "% 10:00", "% 11:00",
                                    "% 12:00", "% 13:00", "% 14:00", "% 15:00", "% 16:00", "% 17:00", "% 18:00", "% 19:00", "% 20:00", "% 21:00", "% 22:00", "% 23:00"};
            try
            {
                std::unique_ptr<sql::PreparedStatement> stmnt1(conn->prepareStatement("select avg(dust) from AIR where date like ? and region = ?"));
                std::unique_ptr<sql::PreparedStatement> stmnt2(conn->prepareStatement("select variance(dust) from AIR where date like ? and region = ?"));
                std::cout << "---------------" << std::endl;
                for(int i=0; i<24; i++)
                {
                    std::cout << i << ":00 미세먼지" << std::endl;
                    stmnt1->setString(1, clock[i]);
                    stmnt1->setString(2, region);
                    sql::ResultSet *res1 = stmnt1->executeQuery();
                    stmnt2->setString(1, clock[i]);
                    stmnt2->setString(2, region);
                    sql::ResultSet *res2 = stmnt2->executeQuery();
                    while (res1->next())
                    {
                        std::cout << "평균 = " << res1->getFloat(1) << std::endl;
                        if(res1->getFloat(1) <= 30)
                        {
                            std::cout << "미세먼지 농도: 좋음" << std::endl;
                        }
                        else if(res1->getFloat(1) <= 80)
                        {
                            std::cout << "미세먼지 농도: 보통" << std::endl;
                        }
                        else if(res1->getFloat(1) <= 150)
                        {
                            std::cout << "미세먼지 농도: 나쁨" << std::endl;
                        }
                        else
                        {
                            std::cout << "미세먼지 농도: 매우 나쁨" << std::endl;
                        }
                    }
                    while (res2->next())
                    {
                        std::cout << "분산 = " << res2->getFloat(1) << std::endl;
                        std::cout << "---------------" << std::endl;
                    }
                }
            }
            catch(sql::SQLException& e)
            {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
            }
        }

    void region_subway(std::unique_ptr<sql::Connection> &conn, std::string region)
        {
            try
            {
                std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("select line from SUBWAY where region = ? group by line"));
                stmnt->setString(1, region);
                sql::ResultSet *res = stmnt->executeQuery();
                while (res->next())
                {
                    std::cout << res->getString(1) << "     ";
                }
                std::cout << std::endl;
            }
            catch(sql::SQLException& e)
            {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
            }
        }

    void subway_avg(std::unique_ptr<sql::Connection> &conn, std::string region, std::string subway)
        {
            try
            {
                std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("select line, avg(time9to12), avg(time13to16), avg(time17to20) from SUBWAY where region = ? and line = ?"));
                stmnt->setString(1, region);
                stmnt->setString(2, subway);
                sql::ResultSet *res = stmnt->executeQuery();
                while (res->next())
                {
                    std::cout << res->getString(1) << std::endl;
                    std::cout << "9시~12시 교통량 평균 값: " << res->getFloat(2) << std::endl;
                    std::cout << "13시~16시 교통량 평균 값: " << res->getFloat(3) << std::endl;
                    std::cout << "17시~20시 교통량 평균 값: " << res->getFloat(4) << std::endl;
                }
                std::cout << std::endl;
            }
            catch(sql::SQLException& e)
            {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
            }
        }    

    void showSubWay(std::unique_ptr<sql::Connection> &conn, std::string mydate,std::string myregion,std::string mystation) {
        try {
        // createStaemet 객체 생성
        std::unique_ptr<sql::Statement> sway(conn->createStatement());
        std::string zp;
        sql::ResultSet *tes=sway->executeQuery("SELECT DISTINCT LINE,STATION FROM SUBWAY WHERE REGION= '"+myregion+"'");
     while (tes->next())
        {
            std::cout<<"호선: "<<tes->getString(1);
            std::cout<<", 역이름: "<<tes->getString(2)<<std::endl;
        }
        std::cout<<"\n 혹시 찾는 역이 없습니까? y or n\n";
        getline(std::cin,zp);
        
        if(zp=="y" || zp=="Y")
        {
            std::string pp="NO%";
            tes=sway->executeQuery("SELECT LINE,STATION FROM SUBWAY WHERE REGION LIKE '데이터%' GROUP BY STATION");
            
            if(tes->rowsCount()==0)
                std::cout<<"쿼리문제\n";
            while (tes->next())
            {

            std::cout<<"호선: "<<tes->getString(1);
            std::cout<<", 역이름: "<<tes->getString(2)<<std::endl;
            }
        }
        
        
        std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
        // 쿼리를 실행
        mydate=mydate.substr(0,mydate.find('-'));
        //SELECT LINE,STATION,ROUND(AVG(TIME9TO12)) AS '9~12탑승객 평균',ROUND(AVG(TIME13TO16)) AS '13~16탑승객 평균',ROUND(AVG(TIME17TO20)) AS '17~20탑승객 평균' FROM SUBWAY WHERE MONTH LIKE '%05%' AND LINE = (SELECT DISTINCT LINE FROM SUBWAY WHERE STATION ='서울대입구') GROUP BY STATION
        sql::SQLString aa="SELECT DISTINCT LINE FROM SUBWAY WHERE STATION = '"+ mystation+"'";
        sql::ResultSet *res = stmnt->executeQuery(aa);
        std::string myline;
        int rowcount=res->rowsCount();
        while (res->next())
        {
            std::cout << "호선 이름 = " << res->getString(1)<<std::endl;
            if(rowcount==1)
            {
                myline=res->getString(1);
            }
        }
        
        if(rowcount !=1)
        {
            std::cout<<"몇 호선을 타시겠습니까? ";
            getline(std::cin,myline);          
        }        
        
        
        sql::SQLString date = "SELECT LINE,STATION,ROUND(AVG(TIME9TO12)) AS '9~12탑승객 평균',ROUND(AVG(TIME13TO16)) AS '13~16탑승객 평균',ROUND(AVG(TIME17TO20)) AS '17~20탑승객 평균' FROM SUBWAY WHERE MONTH LIKE";
        sql::SQLString when =" '%"+mydate+"%'";
        sql::SQLString region = " AND LINE ='"+myline+"'";
        sql::SQLString rest =" GROUP BY STATION";
        
        res=stmnt->executeQuery(date+when+region+rest);    
    
        if(res->rowsCount()==0)
            std::cout<<"데이터없음"<<std::endl;
        // std::cout<<res->rowsCount()<<std::endl;
        while (res->next()) 
        {
            
            std::cout << "LINE = " << res->getString(1);
            std::cout << ", STATION = " << res->getString(2);
            std::cout<< ", \t\t9~12탑승객 평균= " <<res->getInt(3);
            std::cout<< ", 13~16탑승객 평균 = " <<res->getInt(4);
            std::cout<< ", 17~20탑승객 평균 = " <<res->getInt(5)<<std::endl<<std::endl;
        }   

        
        res=stmnt->executeQuery("select AVG(RP_PER) from WEATHER WHERE DATE LIKE"+when);
        while (res->next())
            {
                std::cout<<"날짜: "+mydate<<" 강수확률: "<<res->getFloat(1)<<std::endl; 
            }
    
    // 실패시 오류 메세지 반환
    } catch(sql::SQLException& e){
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }

    }

    void showPlaceAndDust(std::unique_ptr<sql::Connection> &conn, std::string mydate, std::string myregion) {
        try {
            // createStaemet 객체 생성
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            
            // 쿼리를 실행
            sql::SQLString date = "SELECT A.REGION,FESTIVAL.BDATE,FESTIVAL.EDATE,FESTIVAL.ENAME,A.DUST,A.SDUST FROM FESTIVAL INNER JOIN (SELECT NUMR ,MIN(DATE) AS DATE ,REGION, AVG(DUST) AS DUST,AVG(SDUST) AS SDUST FROM AIR WHERE DATE LIKE ";
            sql::SQLString when =" '%-"+mydate+" %'";
            sql::SQLString region = " AND REGION = '"  + myregion+ "' ";
            sql::SQLString rest =")AS A ON FESTIVAL.NUMR =A.NUMR ";

            sql::ResultSet *res = stmnt->executeQuery(date+when+region+rest);
            // 반복문을 통해서 내부의 값을 반환
            
            while(res->rowsCount()==0){
                std::cout<<"찾은 데이터가 없습니다"<<std::endl;
                std::cout<<"시간을입력해주세요:ex)month-date ";
                getline(std::cin,mydate);
                sql::SQLString when =" '%-"+mydate+" %'";
                std::cout<<"장소을입력해주세요: ";
                getline(std::cin,myregion);
                sql::SQLString region = " AND REGION = '"  + myregion+ "' ";
                res = stmnt->executeQuery(date+when+region+rest);
            }
            std::cout<<std::endl;
            while (res->next()) 
            {
                std::cout<< "미세먼지 예상치 = " <<res->getFloat(5);
                std::cout<< ", 초미세먼지 예상치 = " <<res->getFloat(6)<<std::endl<<std::endl;
                std::cout << "지역 = " << res->getString(1);
                std::cout << ", 시작 날짜 = " << res->getString(2);
                std::cout << ", 끝나는 날짜 = " << res->getString(3);
                std::cout << ", 행사이름 = " << res->getString(4)<<std::endl<<std::endl;
            
            } 
            
            
            
        // 실패시 오류 메세지 반환
        } catch(sql::SQLException& e){
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
    }



    void region_dust(std::unique_ptr < sql::Connection > &conn,std::string date)
    {
    std::vector<int> a;
    try{
        std::unique_ptr < sql::Statement > stmnt(conn -> createStatement());
        float fj;
        sql::ResultSet *res = stmnt -> executeQuery("select * FROM AIR WHERE DATE LIKE'%"+date+" %'" );
    
     while (res->next()) {
            
            // DUST -= STDDEV_DUST
            a.push_back(res ->getInt(4));
            }
    
    res=stmnt->executeQuery("SELECT * FROM STDDEV_DUST");
    while (res->next()) {
            fj=res->getFloat(1);
            
        }
    std::vector<int>::iterator iter;
    int dust=0;
    int count=0;
    for ( iter=a.begin(); iter!=a.end() ; ++iter)
    {
        dust += *iter;
        count++;
    }
    dust=dust/count;
    if(dust-fj>=0)
    {
    std:: cout << "오늘의 미세먼지는 평소보다  " << round(dust-fj) << "많습니다." << std::endl;
    }
    else
    {
        std:: cout << "오늘의 미세먼지는 평소보다  " << round(dust-fj) << "적습니다." << std::endl;
    }

    }catch(sql::SQLException& e){
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
   }

    }
  void showResort(std::unique_ptr < sql::Connection > &conn,std::string region)
  {
    std::unique_ptr < sql::Statement > stmnt(conn -> createStatement());
    sql::ResultSet *res =stmnt->executeQuery("SELECT * FROM JOINRESORT WHERE REGION ='"+region+"'");
    while (res->next())
    {
        std::cout<<"지역: "<<res->getString(1);
        std::cout<<",\t 현재 영업 상태: "<<res->getString(3);
        std::cout<<",\t 주소: "<<res->getString(4);
        std::cout<<",\t\t 도로명주소: "<<res->getString(5);
        std::cout<<",\t\t 업체 명: "<<res->getString(7)<<std::endl<<std::endl;    
    }
    
  }

};
    


int main()
{
    std::string region;
    try
    {
        sql::Driver* driver= sql::mariadb::get_driver_instance();
        // 연결할 DB의 특정 IP, DB를 정의
        sql::SQLString url=("jdbc:mariadb://10.10.21.122/studyakm");        
        // 연결할 DB를 사용할 유저를 정의
        sql::Properties properties({ {"user", "JH"}, {"password", "1234"} });
            
        // sql::Properties properties({ {"user", "JH"}, {"password", "1234"} });
        
        // 객체에 값을 통하여 연결을 시도        
        std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
        fiveteam a;
        std::string date;
        std::string region;
        //std::cout<<"서울 문화행사를 보여드릴게요.\n";
        std::cout<<"예시:05-01\n"<<"원하는 날짜를 입력해주세요: ";
        getline(std::cin,date);
        std::cout<<"예시: 강남구 \n"<<"원하는 지역을 입력해주세요: ";
        getline(std::cin,region);
        std::cout<<std::endl<<region<<"의 있는 숙박업소입니다."<<std::endl<<std::endl;
        a.showResort(conn,region);
        std::cout<<std::endl;
        std::cout<<"탑승하실 역 이름을 입력해주세요: ";
        std::string train;
        getline(std::cin,train);
        a.subway_avg(conn,region,train);
        std::cout<<"\n그날 미세먼지 ";
        a.dustavg(conn,region);
        a.region_dust(conn,date);
        std::string yes;
        std::cout<<"모든 정보를 한번에 보시겠습니까? y or n";
        getline(std::cin,yes);
        if(yes =="y" || yes == "Y"){
        a.showPlaceAndDust(conn,date,region);        
        a.showSubWay(conn,date,region,train);
        }

    }
    // 연결 실패시 오류 발생 
    catch(sql::SQLException& e)
    {
        std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
        // 프로그램 비정상 종료
        return 1;
    }
    
    return 0;
}