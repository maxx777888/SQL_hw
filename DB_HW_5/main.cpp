#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <regex>
#include <sstream>


class MyDataBase 
{
public:
	MyDataBase(std::string password):c("host=localhost port=5432 dbname=PhoneClient user=postgres password='" + password + "'") {
		std::cout << "Сonnection with the database is established." << std::endl;
	}
	MyDataBase() = delete;

	//1.Метод, создающий структуру БД (таблицы)
	void create_table() {
		try {
			pqxx::work tx(c);
			
				tx.exec("CREATE TABLE IF NOT EXISTS ClientInfo (id SERIAL PRIMARY KEY, "
					"first_name VARCHAR(50) NOT NULL, last_name VARCHAR(50) NOT NULL,"
					"email VARCHAR(50) UNIQUE CHECK(email LIKE '%_@__%.__%')); "
					"CREATE TABLE IF NOT EXISTS PhoneInfo (id SERIAL PRIMARY KEY, "
					"id_client integer REFERENCES ClientInfo(id), phone_number VARCHAR(20))");
				tx.commit();
				std::cout << "Table created successfully" << std::endl;
			
		}
		catch (const std::exception& e) {
			std::cout << "There were problems in the process of creating a table!!! " << e.what() << std::endl;
		}
	}


	//2.Метод, позволяющий добавить нового клиента
	void add_new_client() {
		try {
			//Получаем данные клиента
			std::cout << "To create a new client you need to enter -> first name, "
				"last name, email and phone number in turn." << std::endl;
			std::string f_name, l_name, e_mail, phone_num;
			std::cout << "Enter First Name:" << std::endl;
			std::cin >> f_name;
			std::cout << "Enter Last Name:" << std::endl;
			std::cin >> l_name;
			std::cout << "Enter Email:" << std::endl;
			e_mail = get_email();

			do {
				std::cout << "Enter the phone number(10 digits without spaces) or NO(No phone):" << std::endl;
				std::cin >> phone_num;
			} while (!(phone_num.length() == 10) && (phone_num != "NO"));

			if (phone_num == "NO")
			{
				phone_num = "NULL";
			}
			else {
				phone_num = phone_number_pattern(phone_num);
			}

			//Подготовка шаблона для отправки в базу данных
			c.prepare("insert_client_info", "INSERT INTO ClientInfo (first_name, last_name, email) "
				"VALUES ($1, $2, $3)");
			c.prepare("insert_phone_info", "INSERT INTO PhoneInfo (id_client, phone_number) "
				"VALUES ($1, $2)");
			//Запрос на добавление в таблицу ClientInfo
			pqxx::work tx(c);
			tx.exec_prepared("insert_client_info", f_name, l_name, e_mail);
			tx.commit();
			//Запрос на добавление в таблицу PhoneInfo
			pqxx::work tx1(c);

			//Получаем последний созданный id_client
			int id_client = tx1.query_value<int>("SELECT id FROM ClientInfo ORDER BY id DESC LIMIT 1");

			//std::cout << id_client << std::endl;
			tx1.exec_prepared("insert_phone_info", id_client, phone_num);
			tx1.commit();

			std::cout << "New Client added successfully." << std::endl;
		}
		catch (const std::exception& e) {
			std::cout << "There were problems in the process of adding a new client info!!! " << e.what() << std::endl;
		}
	}

	//3.Метод, позволяющий добавить телефон для существующего клиента
	void add_new_phone() {
		try {

			std::string e_mail, phone_num;
			std::cout << "Enter email of the person who you want to add a new phone number:" << std::endl;
			e_mail = get_email();

			pqxx::work tx(c);
			pqxx::result res = tx.exec("SELECT id FROM ClientInfo WHERE email=" + tx.quote(e_mail));

			if (res.empty()) {
				tx.commit();
				std::cout << "There is no such registered email in the table!!! " << std::endl;
				return;
			}

			int id_client = res[0][0].as<int>();


			pqxx::result r = tx.exec("SELECT phone_number "
				"FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));

			if (r.empty())
			{

			}
			else if (r[0][0].as<std::string>() == "NULL") {
				tx.exec("DELETE FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
			}

			phone_num = get_phone();

			//Подготовка шаблона для отправки в базу данных
			c.prepare("insert_phone_info", "INSERT INTO PhoneInfo (id_client, phone_number) "
				"VALUES ($1, $2)");

			//Запрос на добавление в таблицу PhoneInfo
			tx.exec_prepared("insert_phone_info", id_client, phone_num);
			tx.commit();

			std::cout << "New Phone Number added successfully" << std::endl;


		}
		catch (const std::exception& e) {
			std::cout << "There were problems in the process of adding a new phone!!! " << e.what() << std::endl;
		}
	}

	//4.Метод, позволяющий изменить данные о клиенте
	void change_client_data() {

		std::string e_mail;
		std::cout << "Enter the email address of the client from whom you want to replace the information:" << std::endl;
		e_mail = get_email();

		try {
			pqxx::work tx(c);
			pqxx::result res = tx.exec("SELECT id FROM ClientInfo WHERE email='" + tx.esc(e_mail) + "'");

			if (res.empty()) {
				tx.commit();
				std::cout << "There is no such registered email in the table!!!" << std::endl;
				return;
			}
			int c_choice;
			do {
				std::cout << "Enter what data you want to change from the client: First name(1) Last name (2) Email(3)" << std::endl;
				std::cin >> c_choice;

			} while (c_choice != 1 && c_choice != 2 && c_choice != 3);

			std::string change_field = "email";
			switch (c_choice)
			{
			case 1:
				change_field = "first_name";
				break;
			case 2:
				change_field = "last_name";
				break;
			default:
				break;
			}

			std::string new_data_str;
			std::cout << "Enter new data" << std::endl;
			std::cin >> new_data_str;

			c.prepare("update_client_info", "UPDATE ClientInfo SET " + change_field + "=$1 WHERE email=$2");

			//Запрос на добавление в таблицу PhoneInfo
			tx.exec_prepared("update_client_info", new_data_str, e_mail);
			tx.commit();

			std::cout << "The data modification process was completed successfully!" << std::endl;

		}
		catch (const std::exception& e)
		{
			std::cout << "There were problems in the process of changing client info!!! " << e.what() << std::endl;
		}

	}
	//5.Метод, позволяющий удалить телефон для существующего клиента
	void delete_phone_client() {

		std::string e_mail;
		std::cout << "Enter the email address of the client from whom you want to delete the phone number:" << std::endl;
		e_mail = get_email();
		try {
			pqxx::work tx(c);
			pqxx::result res = tx.exec("SELECT id FROM ClientInfo WHERE email='" + tx.esc(e_mail) + "'");

			if (res.empty()) {
				tx.commit();
				std::cout << "There is no such registered email in the table!!!" << std::endl;
				return;
			}
			int id_client = res[0][0].as<int>();
			std::string phone_num;
			pqxx::result a_phones = tx.exec("SELECT COUNT(phone_number) FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
			if (a_phones[0][0].as<int>() > 1)
			{
				bool phone_exist = false;
				do {
					phone_num = get_phone();
					pqxx::result phone_to_delete = tx.exec("SELECT id FROM PhoneInfo WHERE phone_number=" + tx.quote(phone_num));
					if (phone_to_delete.empty())
					{
						std::cout << phone_num << " is missing in the database, enter another number to delete" << std::endl;
					}
					else {
						tx.exec("DELETE FROM PhoneInfo WHERE phone_number=" + tx.quote(phone_num));
						std::cout << "This phone number " << phone_num << " has been deleted." << std::endl;
						phone_exist = true;
					}

				} while (!phone_exist);
			}
			else {
				tx.exec("DELETE FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
				std::cout << "The phone number has been deleted." << std::endl;
			}
			tx.commit();

		}
		catch (const std::exception& e)
		{
			std::cout << "There were problems in the process of deleting phone number!!! " << e.what() << std::endl;
		}
	}

	//6.Метод, позволяющий удалить существующего клиента
	void delete_existing_client() {
		std::string e_mail;
		std::cout << "Enter the email address of the client from whom you want to delete from the table:" << std::endl;
		e_mail = get_email();
		try
		{
			pqxx::work tx(c);
			pqxx::result res = tx.exec("SELECT id FROM ClientInfo WHERE email='" + tx.esc(e_mail) + "'");

			if (res.empty()) {
				tx.commit();
				std::cout << "There is no such registered email in the table!!!" << std::endl;
				return;
			}
			int id_client = res[0][0].as<int>();
			tx.exec("DELETE FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
			tx.exec("DELETE FROM ClientInfo WHERE id=" + tx.quote(id_client));
			tx.commit();
			std::cout << "The client has been deleted." << std::endl;


		}
		catch (const std::exception& e)
		{
			std::cout << "There were problems in the process of deleting client!!! " << e.what() << std::endl;
		}
	}

	//7.Метод, позволяющий найти клиента по его данным (имени, фамилии, email-у или телефону)
	void client_search() {
		int c_choice;
		do {
			std::cout << "Enter how you want to search for the client in the database: Email(1) OR Phone(2) " << std::endl;
			std::cin >> c_choice;

		} while (c_choice != 1 && c_choice != 2);

		try
		{
			std::string search;
			pqxx::work tx(c);
			pqxx::result res;

			if (c_choice == 1)
			{
				std::cout << "Enter the email address of the client:" << std::endl;
				search = get_email();

				res = tx.exec("SELECT id FROM ClientInfo WHERE email='" + tx.esc(search) + "'");

				if (res.empty()) {
					tx.commit();
					std::cout << "There is no such registered email in the table!!!" << std::endl;
					return;
				}
			}
			else if (c_choice == 2) {
				search = get_phone();

				res = tx.exec("SELECT id_client FROM PhoneInfo "
					"WHERE phone_number='" + tx.esc(search) + "'");

				if (res.empty()) {
					tx.commit();
					std::cout << "There is no such registered phone number in the table!!!" << std::endl;
					return;
				}

			}

			int id_client = res[0][0].as<int>();

			for (auto [first_name, last_name, email] : tx.query<std::string, std::string, std::string>("SELECT "
				"first_name, last_name, email FROM ClientInfo WHERE id=" + tx.quote(id_client)))
			{
				std::cout << "Client's First name is " << first_name << std::endl;
				std::cout << "Client's Last name is " << last_name << std::endl;
				std::cout << "Client's Email is " << email << std::endl;
			}


			pqxx::result a_phones = tx.exec("SELECT COUNT(phone_number) FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
			if (a_phones[0][0].as<int>() > 1)
			{
				int count = 1;
				for (auto [phone_info] : tx.query<std::string>("SELECT phone_number FROM PhoneInfo "
					"WHERE id_client=" + tx.quote(id_client)))
				{
					std::cout << count << ") " << "phone number " << phone_info << std::endl;
					count++;
				}
			}
			else {
				pqxx::result a_phone = tx.exec("SELECT phone_number FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
				if (a_phone.empty() || a_phone[0][0].as<std::string>() == "NULL")
				{
					std::cout << "The client does't have a registered phone number" << std::endl;
				}
				else {
					std::string phone = tx.query_value<std::string>("SELECT phone_number "
						"FROM PhoneInfo WHERE id_client=" + tx.quote(id_client));
					std::cout << "Client's phone number is " << phone << std::endl;
				}
			}
			tx.commit();

		}
		catch (const std::exception& e)
		{
			std::cout << "There were problems in the process of searching client!!! " << e.what() << std::endl;
		}
	}


private:

	pqxx::connection c;
	

	//Метод проверки на правильность ввода формата электронной почты
	bool check_email(std::string e) {

		std::regex pat(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
		if (std::regex_match(e, pat))
		{
			return true;
		}
		return false;
	}

	//Метод получения электронной почты
	std::string get_email() {
		std::string email;
		do {
			std::cin >> email;
			if (!check_email(email)) {
				std::cout << "The email format is incorrect!" << std::endl;
			}
		} while (!check_email(email));
		return email;
	}

	//Метод преобразует строку в формат телефонного номера
	std::string phone_number_pattern(std::string p) {

		std::stringstream ss;
		ss << "+7(" << p.substr(0, 3) << ") " << p.substr(3, 3) << "-" << p.substr(6, 2) << "-" << p.substr(8);
		std::string formatted_phone_number = ss.str();
		return formatted_phone_number;
	}

	//Метод получения номера телефона
	std::string get_phone() {
		std::string phone;
		do {
			std::cout << "Enter the phone number(10 digits without spaces):" << std::endl;
			std::cin >> phone;
		} while (!(phone.length() == 10));
		return phone_number_pattern(phone);
	}

	

};


int main() {

	try {
		//Нужно вставить пароль для подключения к базе данных
		std::string pasw;
		std::cout << "Enter password to the datebase: ";
		std::cin >> pasw;
		MyDataBase db(pasw);
		db.create_table();
		std::cout << std::endl;
		bool exit = false;
		while (!exit) {
			int user_choice;
			std::cout << "Select the option what you want to do with the database." << std::endl;
			std::cout << "Add a new clint(1), Add a new phone number(2), change a cliet information(3), " << std::endl;
			std::cout << "delete an existing client(4), delete a phone number(5), search for a client info(6) "
				"or some other number to exit. " << std::endl;

			std::cin >> user_choice;

			switch (user_choice) 
			{
			case 1:
				db.add_new_client();
				break;
			case 2:
				db.add_new_phone();
				break;
			case 3:
				db.change_client_data();
				break;
			case 4:
				db.delete_existing_client();
				break;
			case 5:
				db.delete_phone_client();
				break;
			case 6:
				db.client_search();
				break;
			default:
				exit = true;
				break;
			}
			std::cout << std::endl;
		}
		
	}
	catch (const std::exception& e)
	{
		std::cout << "Error when connecting to database!!!" << e.what() << std::endl;
	}
	

	return 0;
}