#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/collection.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <string>
#include <windows.h>

class Book;
class Sale;
class Stock;
class Shop;

class Publisher {
public:
    std::string name = "";

    Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

    template<typename Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");

    }
};

class Book {
public:
    std::string title = "";
    Wt::Dbo::ptr<Publisher> publisher;
    Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

    template<typename Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, title, "title");
        Wt::Dbo::belongsTo(a, publisher, "publisher");
        Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "book");
    }
};

class Shop {
public:
    std::string name = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;


    template<typename Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "shop");
    }
};

class Stock {
public:
    int count;
    Wt::Dbo::ptr<Book> book;
    Wt::Dbo::ptr<Shop> shop;
    Wt::Dbo::collection<Wt::Dbo::ptr<Sale>> sales;

    template<typename Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, count, "count");
        Wt::Dbo::belongsTo(a, book, "book");
        Wt::Dbo::belongsTo(a, shop, "shop");
        Wt::Dbo::hasMany(a, sales, Wt::Dbo::ManyToOne, "stock");
    }
};

class Sale {
public:
    std::string date;
    int price;
    int count;
    Wt::Dbo::ptr<Stock> stock;


    template<typename Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, date, "date");
        Wt::Dbo::field(a, price, "price");
        Wt::Dbo::field(a, count, "count");
        Wt::Dbo::belongsTo(a, stock, "stock");

    }
};

//Метод для заполнения таблицы данными
void enter_new_data_to_the_created_db(Wt::Dbo::Session& session) {

    //Создаем транзакцию
    Wt::Dbo::Transaction transaction(session);

    //Добавляем новые данные в таблицу Publisher
    std::unique_ptr<Publisher> p1 = std::make_unique<Publisher>();
    p1->name = "English club";
    auto ptr_pub1 = session.add(std::move(p1));

    std::unique_ptr<Publisher> p2 = std::make_unique<Publisher>();
    p2->name = "AST";
    auto ptr_pub2 = session.add(std::move(p2));

    //Добавляем новые данные в таблицу Book
    std::unique_ptr<Book> b1 = std::make_unique<Book>();
    b1->title = "Around the World in 80 Days";
    b1->publisher = ptr_pub1;
    auto ptr_book1 = session.add(std::move(b1));

    std::unique_ptr<Book> b2 = std::make_unique<Book>();
    b2->title = "The Mysterious Island";
    b2->publisher = ptr_pub1;
    auto ptr_book2 = session.add(std::move(b2));

    std::unique_ptr<Book> b3 = std::make_unique<Book>();
    b3->title = "Brave New World";
    b3->publisher = ptr_pub1;
    auto ptr_book3 = session.add(std::move(b3));

    std::unique_ptr<Book> b4 = std::make_unique<Book>();
    b4->title = "Treasure Island";
    b4->publisher = ptr_pub2;
    auto ptr_book4 = session.add(std::move(b4));

    std::unique_ptr<Book> b5 = std::make_unique<Book>();
    b5->title = "The Hound of the Baskervilles";
    b5->publisher = ptr_pub2;
    auto ptr_book5 = session.add(std::move(b5));

    std::unique_ptr<Book> b6 = std::make_unique<Book>();
    b6->title = "The A B C Murders";
    b6->publisher = ptr_pub2;
    auto ptr_book6 = session.add(std::move(b6));

    //Добавляем новые данные в таблицу Shop
    std::unique_ptr<Shop> s1 = std::make_unique<Shop>();
    s1->name = "LitRes";
    auto ptr_shop1 = session.add(std::move(s1));

    std::unique_ptr<Shop> s2 = std::make_unique<Shop>();
    s2->name = "Book Maze";
    auto ptr_shop2 = session.add(std::move(s2));

    std::unique_ptr<Shop> s3 = std::make_unique<Shop>();
    s3->name = "Read-the city";
    auto ptr_shop3 = session.add(std::move(s3));

    //Добавляем новые данные в таблицу Stock
    std::unique_ptr<Stock> t1 = std::make_unique<Stock>();
    t1->book = ptr_book1;
    t1->shop = ptr_shop1;
    t1->count = 100;
    auto ptr_stock1 = session.add(std::move(t1));

    std::unique_ptr<Stock> t2 = std::make_unique<Stock>();
    t2->book = ptr_book2;
    t2->shop = ptr_shop1;
    t2->count = 200;
    auto ptr_stock2 = session.add(std::move(t2));

    std::unique_ptr<Stock> t3 = std::make_unique<Stock>();
    t3->book = ptr_book3;
    t3->shop = ptr_shop1;
    t3->count = 300;
    auto ptr_stock3 = session.add(std::move(t3));

    std::unique_ptr<Stock> t4 = std::make_unique<Stock>();
    t4->book = ptr_book4;
    t4->shop = ptr_shop2;
    t4->count = 150;
    auto ptr_stock4 = session.add(std::move(t4));

    std::unique_ptr<Stock> t5 = std::make_unique<Stock>();
    t5->book = ptr_book5;
    t5->shop = ptr_shop2;
    t5->count = 250;
    auto ptr_stock5 = session.add(std::move(t5));

    std::unique_ptr<Stock> t6 = std::make_unique<Stock>();
    t6->book = ptr_book6;
    t6->shop = ptr_shop3;
    t6->count = 500;
    auto ptr_stock6 = session.add(std::move(t6));

    //Добавляем новые данные в таблицу Sale
    std::unique_ptr<Sale> l1 = std::make_unique<Sale>();
    l1->date = "05-07-2023";
    l1->price = 500;
    l1->count = 50;
    l1->stock = ptr_stock1;
    session.add(std::move(l1));

    std::unique_ptr<Sale> l2 = std::make_unique<Sale>();
    l2->date = "05-07-2023";
    l2->price = 350;
    l2->count = 60;
    l2->stock = ptr_stock2;
    session.add(std::move(l2));

    std::unique_ptr<Sale> l3 = std::make_unique<Sale>();
    l3->date = "05-07-2023";
    l3->price = 450;
    l3->count = 100;
    l3->stock = ptr_stock3;
    session.add(std::move(l3));

    std::unique_ptr<Sale> l4 = std::make_unique<Sale>();
    l4->date = "05-07-2023";
    l4->price = 550;
    l4->count = 120;
    l4->stock = ptr_stock4;
    session.add(std::move(l4));

    std::unique_ptr<Sale> l5 = std::make_unique<Sale>();
    l5->date = "05-07-2023";
    l5->price = 750;
    l5->count = 90;
    l5->stock = ptr_stock5;
    session.add(std::move(l5));

    std::unique_ptr<Sale> l6 = std::make_unique<Sale>();
    l6->date = "05-07-2023";
    l6->price = 850;
    l6->count = 30;
    l6->stock = ptr_stock6;
    session.add(std::move(l6));

    transaction.commit();
}
//Метод возвращает true если пользователь ввел число
bool is_number(const std::string& s) {
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    try {
        std::string connectionString = "host = localhost "
            "port = 5432 "
            "dbname = Publisher "
            "user = postgres "
            "password = A159159a";

        //Создаем подключение к таблице
        auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(connectionString);
        Wt::Dbo::Session s;
        s.setConnection(std::move(postgres));
        s.mapClass<Publisher>("publisher");
        s.mapClass<Book>("book");
        s.mapClass<Shop>("shop");
        s.mapClass<Stock>("stock");
        s.mapClass<Sale>("sale");

        //Создаем базу данных
        try {
            s.createTables();
            std::cout << "Table created\n";
        }
        catch (const Wt::Dbo::Exception& e) {
            std::cout << "Table already exist, exception: " << e.what() << std::endl;
        }
        //Метод заполняет информацией базу данных
        //enter_new_data_to_the_created_db(s);

        std::string user_ans;
        std::cout << "Enter the publisher's id number:" << std::endl;
        std::cin >> user_ans;
        //Создаем транзакцию
        Wt::Dbo::Transaction t(s);
        //Проверка что пользователь ввел число. 
        if (is_number(user_ans)) 
        {

            int pub_id = s.query<int>("SELECT id FROM publisher").where("id = ?").bind(user_ans);
            //Возвращает 0 если такого id не нашлось в таблице
            if (pub_id == 0) {
                std::cout << "Unfortunately, there is no such publisher in our database!" << std::endl;
            }
            else
            {
                std::string pub_name = s.query<std::string>("SELECT name FROM publisher").where("id = ?").bind(user_ans);
                //Находим книги, которые выпустил издатель
                Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books = s.find<Book>().where("publisher_id = ?").bind(pub_id);
                std::cout << "Information that is stored in our database: " << std::endl;
                std::cout << "Publisher name: " << pub_name << std::endl;
                std::cout << "The publisher has published -> " << books.size() << " books" << std::endl;
                int book_num = 1;
                for (const Wt::Dbo::ptr<Book>& p : books)
                {
                    std::cout << book_num << ") " << p->title << std::endl;
                    book_num++;
                }
                //Находим магазины, в которых продаются книги издателя.
                auto result = s.query < std::string, Wt::Dbo::DirectBinding>("SELECT shop.name "
                    "FROM shop "
                    "JOIN stock ON stock.shop_id = shop.id "
                    "JOIN book ON book.id = stock.id "
                    "JOIN publisher ON publisher.id = book.publisher_id "
                    "WHERE publisher.id =?  GROUP BY shop.name")
                    .bind(user_ans).resultList();

                std::cout << "The publisher has -> " << result.size() << " shops" << std::endl;
                int shop_num = 1;
                for (auto it = result.begin(); it != result.end(); ++it)
                {
                    std::cout << shop_num << ") " << *it << std::endl;
                    shop_num++;
                }

            }
            t.commit();
        }
        else {
            std::cout << "Unfortunately, there is no such publisher in our database!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << "Connection's problem: " << e.what() << std::endl;
    }

}

