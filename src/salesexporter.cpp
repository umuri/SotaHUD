#include "salesexporter.h"
#include "notificationmanager.h"
#include <QFile>
#include <QDir>

SalesExporter::SalesExporter(Config * tempConfig = NULL, notificationManager * tempPopup = NULL)
{

    appConfig = tempConfig;
    popupManager = tempPopup;
    if ( !QDir(QDir::currentPath() + "/Sales").exists() )
    {
        QDir().mkdir(QDir::currentPath() + "/Sales");
    }

   QString yearMonth = QDate::currentDate().toString("yyyy-MM");

    QString salesPath = QDir::currentPath() + "/Sales/" + yearMonth + "-" + appConfig->CharacterName + ".csv";
    salesFile = new QFile(salesPath);

    salesFile->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    salesOut = new QTextStream(salesFile);
}

SalesExporter::~SalesExporter()
{
    salesOut->flush();
    salesFile->close();
}

void SalesExporter::CheckForSale(QString inputString)
{


    QString type="BUY";
    QString item;
    QString price;
    QString vendorLocation;
    QString listedBy;
    QString filledBy;
    QString timestamp;
    QString quantity;
    QRegExp quantityRegExp;
    quantityRegExp.setPattern("x[0-9]*$");
    int start=0;
    int end=0;
    bool found=false;
    if ( inputString.contains("item that you wanted to buy has been fulfilled") ) //We bought
    {
        found=true;
        /*
         * "[c][000000]
    BILL OF EXCHANGE
    ---------------

    Good news! We've received word that your order for an item that you wanted to buy has been fulfilled. The item can be collected at the bank in [b]Owl's Head[/b].

    [b]Order Details:[/b]
     [b][c][8080FF]Gold Crown of the Obsidians x100[-][/c][/b]
     Price: [b][c][E5BC18]220,000 gold[-][/c][/b]
     Vendor Location: [b]Owl's Head[/b]
     Placed by: [b]Umuri Maxwell[/b]
     Fulfilled by: [b]Enrina Spellforce[/b]
     Date of Exchange: 9/30/2017 11:28 AM

    Thank you for your business,

    [b]First Bank of Brittany[/b]
    Branches available in Ardoris, Etceter, Port Graff, and Resolute[-][/c]"

    */

        type = "BUY";
            start=inputString.indexOf("[b][c][8080FF]",start) + 14;
            end=inputString.indexOf("[-][/c][/b]",start);
        item = inputString.mid(start,end-start);
            start=inputString.indexOf("Price: [b][c][E5BC18]",start) + 21;
            end=inputString.indexOf("[-][/c][/b]",start)-5;
        price = inputString.mid(start,end-start);
        price = price.remove(',');
            start=inputString.indexOf("Vendor Location: [b]",start) + 20;
            end=inputString.indexOf("[/b]",start);
        vendorLocation = inputString.mid(start,end-start);
            start=inputString.indexOf("Placed by: [b]",start) + 14;
            end=inputString.indexOf("[/b]",start);
        listedBy = inputString.mid(start,end-start);
            start=inputString.indexOf("Fulfilled by: [b]",start) + 17;
            end=inputString.indexOf("[/b]",start);
        filledBy = inputString.mid(start,end-start);
            start=inputString.indexOf("Date of Exchange:",start) + 18;
            end=inputString.indexOf("Thank you for your business",start)-2;
        timestamp = inputString.mid(start,end-start);

    }
    if ( inputString.contains("item that you listed for sale has been purchased") ) //We sold
    {

        /*"[c][000000]
    BILL OF EXCHANGE
    ---------------

    Good news! We've received word that an item that you listed for sale has been purchased. Enclosed with this bill of exchange, please find the proceeds from this sale.

    [b]Exchange Details:[/b]
     [b][c][8080FF]Gold Crown of the Obsidians x10[-][/c][/b]
     Price: [b][c][E5BC18]24,500 gold[-][/c][/b]
     Vendor Location: [b]Owl's Head[/b]
     Sold by: [b]Umuri Maxwell[/b]
     Purchased by: [b]Iron Master[/b]
     Date: 9/19/2017 1:02 PM

    Thank you for your business,

    [b]First Bank of Brittany[/b]
    Branches available in Ardoris, Etceter, Port Graff, and Resolute[-][/c]"
        */
        found=true;
        type = "SELL";
            start=inputString.indexOf("[b][c][8080FF]",start) + 14;
            end=inputString.indexOf("[-][/c][/b]",start);
        item = inputString.mid(start,end-start);
            start=inputString.indexOf("Price: [b][c][E5BC18]",start) + 21;
            end=inputString.indexOf("[-][/c][/b]",start)-5;
        price = inputString.mid(start,end-start);
        price = price.remove(',');
            start=inputString.indexOf("Vendor Location: [b]",start) + 20;
            end=inputString.indexOf("[/b]",start);
        vendorLocation = inputString.mid(start,end-start);
            start=inputString.indexOf("Sold by: [b]",start) + 12;
            end=inputString.indexOf("[/b]",start);
        listedBy = inputString.mid(start,end-start);
            start=inputString.indexOf("Purchased by: [b]",start) + 17;
            end=inputString.indexOf("[/b]",start);
        filledBy = inputString.mid(start,end-start);
            start=inputString.indexOf("Date: ",start) + 6;
            end=inputString.indexOf("Thank you for your business",start)-2;
        timestamp = inputString.mid(start,end-start);

    }

    if ( item.contains(quantityRegExp))
    {
        quantity = item.mid(item.indexOf(quantityRegExp)+1,-1);
        item = item.mid(0,item.indexOf(quantityRegExp)-1);
    }
    else
    {
        quantity = '1';
    }

    if ( found )
    {
        popupManager->addNotification(type + " mail read from Clipboard into Sales file"," " + type + " \n\r " + item + " | " + quantity +  " | "  + price + " \n\r " + vendorLocation + " | " + listedBy + " \n\r " + filledBy + " | " + timestamp + " | ",3);
        //Add logic to deal with found item
        //Add logic to add to array
        //Add logic to check for duplicates

        //Write it all to file





        *salesOut << '"' << type << '"';
        *salesOut << ",\"" << item << '"';
        *salesOut << ',' << quantity ;
        *salesOut << ',' << price ;
        *salesOut << ",\"" << vendorLocation << '"';
        *salesOut << ",\"" << listedBy << '"';
        *salesOut << ",\"" << filledBy << '"';
        *salesOut << ",\"" << timestamp << '"';
        *salesOut << endl;





        salesOut->flush();

    }


}

