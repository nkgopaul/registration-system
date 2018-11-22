#include "main.h"
#include "constants.h"

MYSQL *conn;

string getSemester(int month) {
    if(month >= 8 && month <= 11) {
        return "Q1";
    } else if (month >= 0 && month <= 2) {
        return "Q2";
    } else if (month >=3 && month <= 5) {
        return "Q3";
    } else {
        return "Q4";
    }
}

vector<string> getNextSemesterYear(string semester, int year) {
    vector<string> v;
    if(semester=="Q2") {
        v = {"Q3", to_string(year)};
    } else if (semester=="Q3") {
        v = {"Q4", to_string(year)};
    } else if (semester=="Q4") {
        v = {"Q1", to_string(year)};
    } else {
        v = {"Q2", to_string(year+1)};     
    }
    return v;
}

void cureCinState() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
}

void login() {
    string username, password, id;
    bool invalidLogin = true;

    cout << "LOGIN MENU" << endl;
    cout << "You will need to enter your credentials to login." << endl;

    while(invalidLogin) { 
        cout << "Please enter your id: " << endl;
        cin >> id;
        cout << "Please enter your password: " << endl;
        cin >> password;

        string query = "SELECT * FROM student WHERE Id=" + id + " AND BINARY Password='" + password + "';"; 
        
        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);

        invalidLogin = res_set==0 || (int)mysql_num_rows(res_set)==0;

        if(invalidLogin) {
            cout << "Invalid login. Please try again." << endl;
        }
        mysql_free_result(res_set);
    }
    student_menu(id);
    return;
}

void student_menu(string id) {
    int choice;
    
    while(choice != 5) {
        cout << "STUDENT MENU" << endl;
        cout << "Current courses:" << endl;
        
        time_t now = time(0);
        tm *ltm = localtime(&now);
        string semester = getSemester(ltm->tm_mon);

        string query = "SELECT UoSCode, Semester, Year FROM transcript WHERE StudId=" + id + " AND Semester='" + semester + "' AND Year=" + to_string(1900+ltm->tm_year) + ";"; 
        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);

        if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
            cout << "No current courses." << endl;
        } else {
            int numrows = (int)mysql_num_rows(res_set);

            for(int i = 0; i < numrows; i++) {
                row = mysql_fetch_row(res_set);
                if(row != NULL) {
                    string uoscode = !row[0] ? "N/A" : row[0];
                    string semester = !row[1] ? "N/A" : row[1];
                    string year = !row[2] ? "N/A" : row[2];
                    cout << "UoSCode: " << uoscode << " Semester: " << semester << " Year: " << year << endl;
                }
            }
        }

        mysql_free_result(res_set);
        cout << "\nEnter your choice:\n1. View my transcript\n2. Enroll in courses\n3. Withdraw from courses\n4. View personal details\n5. Log out" << endl;
        cin >> choice;

        switch(choice) {
            case 1:transcript(id);
            break;
            case 2:enroll(id);
            break;
            case 3:withdraw(id);
            break;
            case 4:personal_details(id);
            break;
            case 5: break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                cureCinState();
        }
    }
}

void transcript(string id) {
    int choice;

    while(choice!=2) {
        cout << "TRANSCRIPT" << endl;
    
        string query = "SELECT * FROM transcript WHERE StudId=" + id + ";";
        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);
        if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
            cout << "No courses on transcript." << endl;
        }
        else {
            int numrows = (int)mysql_num_rows(res_set);

            for(int i = 0; i < numrows; i++) {
                row = mysql_fetch_row(res_set);
                if(row != NULL) {
                    string uoscode = !row[1] ? "N/A" : row[1];
                    string semester = !row[2] ? "N/A" : row[2];
                    string year = !row[3] ? "N/A" : row[3];
                    string grade = !row[4] ? "N/A" : row[4];
                    cout << "UoSCode: " << uoscode << " Semester: " << semester << " Year: " << year << " Grade: " << grade << endl;
                }
            }
        }

        mysql_free_result(res_set);
        cout << "Enter your choice:\n1. Get course details.\n2. Go back to STUDENT MENU." << endl;
        cin >> choice;
        switch(choice) {
            case 1:course_details(id);
            break;
            case 2: break;
                default: cout << "Invalid input. Please try again." << endl;
                cureCinState();
        }
    }
}

void course_details(string id) {
    string input;
    cout << "COURSE DETAILS" << endl;
    while(input!="2") {  
        cout << "Enter a course code OR enter 2 to go back to TRANSCRIPT:" << endl;
        cin >> input;
        if(input=="2") {
            break;
        }
        string query = "select * from (select t.StudId, t.UoSCode,t.Semester, t.Year, P.UoSName, P.Enrollment, P.MaxEnrollment, P.Name as Lecturer_Name from transcript as t, (select q.UosCode, q.UoSName, q.Semester, q.Year, q.Enrollment, q.MaxEnrollment, L.Id, L.Name from (select * from unitofstudy c natural join uosoffering o where c.UoSCode = o.UoSCode) as q, faculty as L where q.InstructorId = L.Id) as P where t.UoSCode = P.UoSCode and t.Semester = P.Semester and t.Year = P.Year) as a where StudId=" + id + " and UoSCode='" + input + "';";

        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);
        if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
            cout << "Course not found." << endl;
        } else {
            int numrows = (int)mysql_num_rows(res_set);

            for(int i = 0; i < numrows; i++) {
                row = mysql_fetch_row(res_set);
                if(row != NULL) {
                    string uoscode = !row[0] ? "N/A" : row[0];
                    string title = !row[1] ? "N/A" : row[1];
                    string semester = !row[2] ? "N/A" : row[2];
                    string year = !row[3] ? "N/A" : row[3];
                    string enrollment = !row[4] ? "N/A" : row[4];
                    string max_enrollment = !row[5] ? "N/A" : row[5];
                    string lecturer_name = !row[6] ? "N/A" : row[6];
                    string grade = !row[7] ? "N/A" : row[7];
                    cout << "UoSCode: " << uoscode << "\nCourse Title: " << title << "\nSemester: " << semester << "\nYear: " << year << "\nEnrollment: " << enrollment << "\nMax Enrollment: " << max_enrollment << "\nLecturer Name: " << lecturer_name << "\nGrade: " << grade << endl;
                }
            }
        }

        mysql_free_result(res_set);
    }
}

void enroll(string id) {
    string input, choice;
    string course_in, semester_in, year_in;

    cout << "ENROLL" << endl;

    while(input!="2") {
        cout << "Here are the current course offerings:" << endl;

        time_t now = time(0);
        tm *ltm = localtime(&now);
        string semester = getSemester(ltm->tm_mon);
        vector<string> currentSemester = {semester, to_string(1900+ltm->tm_year)};
        vector<string> nextSemester = getNextSemesterYear(semester, 1900+ltm->tm_year);
        struct tm  tstruct;
        char currTimeFormatted[80];
        tstruct = *localtime(&now);
        strftime(currTimeFormatted, sizeof(currTimeFormatted), "%Y-%m-%d", &tstruct);
        
        string query = "SELECT UoSCode, Semester, Year, Enrollment, MaxEnrollment, Name FROM uosoffering JOIN faculty ON Id=InstructorId WHERE (Semester='" + currentSemester[0] + "' AND Year=" + currentSemester[1] + ") OR (Semester='" + nextSemester[0] + "' AND Year=" + nextSemester[1] + ");";

        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);
    
        if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
            cout << "No courses to enroll in." << endl;
        } else {
            int numrows = (int)mysql_num_rows(res_set);
            for(int i = 0; i < numrows; i++) {
                row = mysql_fetch_row(res_set);
                if(row != NULL) {
                    string uoscode = !row[0] ? "N/A" : row[0];
                    string semester = !row[1] ? "N/A" : row[1];
                    string year = !row[2] ? "N/A" : row[2];
                    string enrollment = !row[3] ? "N/A" : row[3];
                    string max_enrollment = !row[4] ? "N/A" : row[4];
                    string lecturer_name = !row[5] ? "N/A" : row[5];
                    cout << "Course Number: " << uoscode << " Semester: " << semester << " Year: " << year << " Enrollment: " << enrollment << " Max Enrollment: " << max_enrollment << " Lecturer: " << lecturer_name << endl;
                }
            }
        }
        mysql_free_result(res_set);

        cout << "Enter 1 to enroll in a course OR 2 to go back to STUDENT MENU." << endl;
        cin >> input;

        if(input=="2") {
            break;
        }
        if(input!="1") {
            cout << "Choice invalid. Please try again." << endl;
            continue;
        }

        cout << "Enter the quarter for enrollment" << endl;
        cout << "1. " << currentSemester[0] << " " << currentSemester[1] << endl;
        cout << "2. " << nextSemester[0] << " " << nextSemester[1] << endl;
        cin >> choice;

        if(choice=="1") {
            semester_in = currentSemester[0];
            year_in = currentSemester[1];
        } else if (choice=="2") {
            semester_in = nextSemester[0];
            year_in = nextSemester[1];
        } else {
            cout << "It looks like you entered a wrong choice. Please try again." << endl;
            continue;
        }

        cout << "Enter the code of the course you would like to enroll in." << endl;
        cin >> course_in;

        query = "CALL enroll(" + id + ", '" + course_in + "', '" + semester_in + "', " + year_in + ", '" + currTimeFormatted + "', @flag);";

        int status = mysql_query(conn, query.c_str());
        if(status!=0) {
            cout << "Failed to enroll." << endl;
        } else {
            mysql_query(conn, "SELECT @flag");
            res_set = mysql_store_result(conn);
            row = mysql_fetch_row(res_set);
            string flag = row[0];
            mysql_free_result(res_set);

            if (flag =="10"){
                cout<<"Enrollment in course "<<course_in<<" successful!"<<endl;
            } else if (flag == "3"){
                cout <<"Prerequisites for course_in "<<course_in<<" not met! Prerequisites are: "<<endl;
                string query1 = "SELECT * FROM requires where UoSCode = '" +course_in+ "';";
                MYSQL_RES *res_set1;
                MYSQL_ROW row1;
                mysql_query(conn, query1.c_str());
                res_set1 = mysql_store_result(conn);
                if(!(res_set1==0 || (int)mysql_num_rows(res_set1)==0)) {
                    int prereq = (int)mysql_num_rows(res_set1);
                    for(int i = 0; i < prereq; i++) {
                        row1 = mysql_fetch_row(res_set1);
                        if(row1 != NULL) {
                            cout <<row1[1]<< endl;
                        }
                    }
                }
                mysql_free_result(res_set1);
            } else if (flag =="2"){
                cout<<"Seats are full for course "<<course_in<<"!"<<endl;
            } else if (flag =="1"){
                cout<<"You have either already registered for this course or have previously taken this course!"<<endl;
            } else if (flag =="4"){
                cout<<"You entered an invalid course code."<<endl;
            }
        }
    }
}


void withdraw(string id) {
    string input;
    cout << "WITHDRAW" << endl;
    while(input!="2") {
        cout << "Here are the courses you can withdraw from:" << endl;
        
        time_t now = time(0);
        tm *ltm = localtime(&now);
        string semester = getSemester(ltm->tm_mon);
        vector<string> currentSemester = {semester, to_string(1900+ltm->tm_year)};
        vector<string> nextSemester = getNextSemesterYear(semester, 1900+ltm->tm_year);

        string query = "SELECT UoSCode, Semester, Year, Grade FROM transcript WHERE StudId=" + id + " AND Grade IS NULL AND ((Semester='" + currentSemester[0] + "' AND Year=" + currentSemester[1] + ") OR (Semester='" + nextSemester[0] + "' AND Year=" + nextSemester[1] + "));";
        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);

        if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
            cout << "No courses found." << endl;
        } else {
            int numrows = (int)mysql_num_rows(res_set);

            for(int i = 0; i < numrows; i++) {
                row = mysql_fetch_row(res_set);
                if(row != NULL) {
                    string uoscode = !row[0] ? "N/A" : row[0];
                    string semester = !row[1] ? "N/A" : row[1];
                    string year = !row[2] ? "N/A" : row[2];
                    string grade = !row[3] ? "N/A" : row[3];
                    cout << "UoSCode: " << uoscode << " Semester: " << semester << " Year: " << year  << " Grade: " << grade << endl;
                }
            }
        }

        mysql_free_result(res_set);

        cout << "Enter a course number to withdraw from or enter 2 to return to Student Menu." << endl;
        cin >> input;

        if(input=="2") {
            break;
        }
        query = "CALL withdraw(" + id + ", '" + input + "', '" + currentSemester[0] + "', " + currentSemester[1] + ", '" + nextSemester[0] + "', " + nextSemester[1] + ", @found_uoscode, @found_semester, @found_year, @flag);";
        int status = mysql_query(conn, query.c_str());

        if(status!=0) {
            cout << "Failed to withdraw from the selected course." << endl;
        } else {

            mysql_query(conn, "SELECT @flag");
            res_set = mysql_store_result(conn);
            row = mysql_fetch_row(res_set);
            string flag = row[0];
            mysql_free_result(res_set);

            if(flag=="10") {
                cout << "Successfully withdrew from course: " << input << "." << endl;

                mysql_query(conn, "SELECT @found_uoscode, @found_semester, @found_year");
                res_set = mysql_store_result(conn);
                row = mysql_fetch_row(res_set);
                string found_uoscode = row[0];
                string found_semester = row[1];
                string found_year = row[2];
                mysql_free_result(res_set);
                
                query = "SELECT * FROM below_half WHERE UoSCode='" + found_uoscode + "' AND Semester='" + found_semester + "' AND Year= '" + found_year + "';";
                mysql_query(conn, query.c_str());
                res_set = mysql_store_result(conn);

                if(!(res_set==0 || (int)mysql_num_rows(res_set)==0)) {
                    cout << "Warning: Course " << found_uoscode << " is below half enrollment capacity." << endl;
                }
                mysql_free_result(res_set);
            } else if(flag=="1") {
                cout << "Failed to withdraw from the selected course. Please make sure that the course code you entered is valid and that you are eligible to withdraw from it." << endl;
            } else {
                cout << "Failed to withdraw from course." << endl;
            }
        }
    }
}

void personal_details(string id) {
    int choice;
    while(choice!=3) {
        cout << "PERSONAL DETAILS" << endl;
        cout << "Here is your personal information:" << endl;

        string query = "SELECT * FROM student WHERE Id=" + id + ";";

        MYSQL_RES *res_set;
        MYSQL_ROW row;
        mysql_query(conn, query.c_str());
        res_set = mysql_store_result(conn);
        if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
            cout << "Personal information not found." << endl;
        } else {
            row = mysql_fetch_row(res_set);
            string id = !row[0] ? "N/A" : row[0];
            string name = !row[1] ? "N/A" : row[1];
            string address = !row[3] ? "N/A" : row[3];
            cout << "Id: " << id << "\nName: " << name << "\nAddress: " << address << endl;
        }
        mysql_free_result(res_set);

        cout << "Enter your choice:\n1. Change my address\n2. Change my password\n3. Go back to STUDENT MENU." << endl;
        cin >> choice;

        switch(choice) {
            case 1: {
                string newAddr, query;
                int status;
                cout << "Enter new address value or enter 2 to cancel" << endl;
                cin.ignore();
                getline(cin, newAddr);
                if(newAddr=="2") {
                    continue;
                }
                query = "CALL update_address(" + id + ", '" + newAddr + "');";
                status = mysql_query(conn, query.c_str());
                if(status!=0) {
                    cout << "Failed to update address." << endl;
                } else {
                    cout << "Updated address." << endl;
                }
                break;
            }
            case 2: {
                string oldPw, newPw, newPwVerify, query;
                int status;

                cout << "Enter your current password:" << endl;
                cin >> oldPw;

                query = "SELECT * FROM student WHERE Id=" + id + " AND BINARY Password='" + oldPw + "';"; 

                MYSQL_RES *res_set;
                MYSQL_ROW row;
                mysql_query(conn, query.c_str());
                res_set = mysql_store_result(conn);

                if(res_set==0 || (int)mysql_num_rows(res_set)==0) {
                    cout << "Password entered is not valid. Please try again." << endl;
                    mysql_free_result(res_set);
                    break;
                }
                mysql_free_result(res_set);

                cout << "Enter new password:" << endl;
                cin >> newPw;
                cout << "Enter new password again:" << endl;
                cin >> newPwVerify;

                if(newPw!=newPwVerify) {
                    cout << "The passwords do not match, please try again." << endl;
                    break;
                }

                query = "CALL update_password(" + id + ", '" + newPw + "');";
                status = mysql_query(conn, query.c_str());
                if(status!=0) {
                    cout << "Failed to update password, please try again." << endl;
                } else {
                    cout << "Updated password." << endl;
                }
            }
            break;
            case 3: break;
            default:
                cout << "Invalid input, please try again. " << endl;
                cureCinState();
        }
    }
}

int main(int argc, char *argv[]) {
    conn = mysql_init(NULL);
    int choice;

    mysql_real_connect (
        conn,
        "localhost",
        "root",
        "[password]",
        "project3-nudb",
        3306,
        NULL,
        CLIENT_MULTI_RESULTS
    );
    
    cout << "Welcome to the course selection system." << endl;
    
    mysql_query(conn, CREATE_BELOW_HALF_TABLE.c_str());

    while(choice != 2) {
        cout << "Enter your choice:\n1. Login\n2. Exit" << endl;
        cin >> choice;
        switch(choice) {
            case 1: login();
            break;
            case 2: break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                cureCinState();
        }
    }

    mysql_query(conn, DROP_BELOW_HALF_TABLE.c_str());

    mysql_close(conn);
    return 0;
}
