Name : Dekan KILIÇ
Number : 2268886
URL : https://reactapplication-rjdwu.mongodbstitch.com/

I have decided to use React.js for the frontend.I didn't put the add/delete book and user option to main page.Instead, I have created different links for these operations at the header because the view was not good with these on the home page.
On the home page , I have used Swipper for slide show. At the bottom of the swipper, I have listed list of books that are available on my MongoDb.

#### Bonus #### 
The only bonus can be about pagination. I didn't use direct pagination with navigation between pages.Instead , I have decided to use 'view more' button 
to see more all books on the right top of book cards in the home page.

---------------------------------------------------------------------------------------------------
In the project directory, you can run:

### `npm install`
and then
### `npm start`

Runs the app in the development mode.\
Open [http://localhost:3000](http://localhost:3000) to view it in your browser.

---------------------------------------------------------------------------------------------------

exports = function(){
  
    var collection = context.services.get("mongodb-atlas").db("BookTracker").collection("Book");
    return collection.findOne({isFriction : true});
};

> result: 
{
  "_id": {
    "$oid": "628633dccf64a518231c9f4a"
  },
  "bookName": "Harry Potter ve Felsefe Taşı",
  "author": "J.K. Rowling",
  "translator": "Ülkü Tamer",
  "editor": "Danny Richard",
  "cover": "https://i.dr.com.tr/cache/500x400-0/originals/0000000105599-1.jpg",
  "isFriction": true,
  "publisher": "Yapı Kredi Yayınları",
  "yearPublished": "2016",
  "averageRatingOftheBook": "4.8",
  "reviews": [
    "This book is excellent.",
    "This book is perfect."
  ],
  "img": "https://i.dr.com.tr/cache/500x400-0/originals/0000000105599-1.jpg",
  "wallpaper": "https://images8.alphacoders.com/110/1102284.jpg",
  "genres": [
    "Classic",
    "Novel",
    "Polisiye",
    "History",
    "Comic"
  ],
  "description": "This book is about bla bla"
}

---------------------------------------------------------------------------------------------------

exports = function(){
  
    var collection = context.services.get("mongodb-atlas").db("BookTracker").collection("Book");
    return collection.findOne({isFriction : false ,  author :{$size:2}});
};

> result: 
{
  "_id": {
    "$oid": "6286341fcf64a518231c9f58"
  },
  "bookName": "Krallık",
  "author": [
    "Jo Nesbo",
    "Dekan"
  ],
  "translator": "Ülkü Tamer",
  "editor": "Danny Richard",
  "cover": "https://i.dr.com.tr/cache/500x400-0/originals/0001938740001-1.jpg",
  "isFriction": false,
  "publisher": "Yapı Kredi Yayınları",
  "yearPublished": "2016",
  "averageRatingOftheBook": "4.8",
  "reviews": [
    "This book is excellent",
    "Perfect book"
  ],
  "rating": {
    "$numberDouble": "4.3"
  },
  "img": "https://i.dr.com.tr/cache/500x400-0/originals/0001938740001-1.jpg",
  "wallpaper": "https://lh3.googleusercontent.com/-WA2Y0XePUVU/X8AmJkVRrBI/AAAAAAAAgBQ/v8E5wclY06Y9-CRrVjygTE7WA65XWtoGQCLcBGAsYHQ/s16000/Genshin-Impact-1.jpg",
  "genres": [
    "Classic",
    "Novel",
    "Polisiye",
    "History",
    "Comic"
  ],
  "description": "This book is about bla bla"
}

---------------------------------------------------------------------------------------------------

exports = function(){
  
    var collection = context.services.get("mongodb-atlas").db("BookTracker").collection("Book");
    return collection.findOne({translator : {$exists:true}});
};

> result: 
{
  "_id": {
    "$oid": "628633dccf64a518231c9f4a"
  },
  "bookName": "Harry Potter ve Felsefe Taşı",
  "author": "J.K. Rowling",
  "translator": "Ülkü Tamer",
  "editor": "Danny Richard",
  "cover": "https://i.dr.com.tr/cache/500x400-0/originals/0000000105599-1.jpg",
  "isFriction": true,
  "publisher": "Yapı Kredi Yayınları",
  "yearPublished": "2016",
  "averageRatingOftheBook": "4.8",
  "reviews": [
    "This book is excellent.",
    "This book is perfect."
  ],
  "img": "https://i.dr.com.tr/cache/500x400-0/originals/0000000105599-1.jpg",
  "wallpaper": "https://images8.alphacoders.com/110/1102284.jpg",
  "genres": [
    "Classic",
    "Novel",
    "Polisiye",
    "History",
    "Comic"
  ],
  "description": "This book is about bla bla"
}

---------------------------------------------------------------------------------------------------

exports = function(){
  
    var collection = context.services.get("mongodb-atlas").db("BookTracker").collection("Book");
    return collection.findOne({editor : {$exists:true} ,  author :{$size:2}});
};

> result: 
{
  "_id": {
    "$oid": "6286341fcf64a518231c9f58"
  },
  "bookName": "Krallık",
  "author": [
    "Jo Nesbo",
    "Dekan"
  ],
  "translator": "Ülkü Tamer",
  "editor": "Danny Richard",
  "cover": "https://i.dr.com.tr/cache/500x400-0/originals/0001938740001-1.jpg",
  "isFriction": false,
  "publisher": "Yapı Kredi Yayınları",
  "yearPublished": "2016",
  "averageRatingOftheBook": "4.8",
  "reviews": [
    "This book is excellent",
    "Perfect book"
  ],
  "rating": {
    "$numberDouble": "4.3"
  },
  "img": "https://i.dr.com.tr/cache/500x400-0/originals/0001938740001-1.jpg",
  "wallpaper": "https://lh3.googleusercontent.com/-WA2Y0XePUVU/X8AmJkVRrBI/AAAAAAAAgBQ/v8E5wclY06Y9-CRrVjygTE7WA65XWtoGQCLcBGAsYHQ/s16000/Genshin-Impact-1.jpg",
  "genres": [
    "Classic",
    "Novel",
    "Polisiye",
    "History",
    "Comic"
  ],
  "description": "This book is about bla bla"
}

---------------------------------------------------------------------------------------------------

exports = function(){
  
    var collection = context.services.get("mongodb-atlas").db("BookTracker").collection("User");
    return collection.findOne({_id : {$exists:true}});
};

> result: 
{
  "_id": {
    "$oid": "62865376cf64a518231c9f5b"
  },
  "username": "Dekan",
  "numberOfBooksRead": {
    "$numberInt": "2"
  },
  "favoriteBooks": [
    {
      "$numberInt": "0"
    }
  ],
  "averageRating": {
    "$numberDouble": "4.5"
  },
  "reviews": [
    {
      "book_id": {
        "$numberInt": "0"
      },
      "rating": {
        "$numberInt": "5"
      },
      "review_text": "It was very good to read this book"
    },
    {
      "book_id": {
        "$numberInt": "1"
      },
      "rating": {
        "$numberInt": "4"
      },
      "review_text": "I am very appreciated"
    }
  ]
}

---------------------------------------------------------------------------------------------------





