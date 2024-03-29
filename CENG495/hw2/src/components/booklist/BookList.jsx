import React , { useState , useEffect } from 'react';
import PropTypes from 'prop-types';
import './booklist.scss';
import { Swiper , SwiperSlide } from 'swiper/react'; 
import BookCard from '../bookcard/BookCard';
import * as Realm from 'realm-web';

const BookList = props => {

//   const items = BooksData;
  const [items,setItems] = useState([]);

    const getBooks = async () => {
        const app = new Realm.App({ id: "reactapplication-rjdwu" });
        const credentials = Realm.Credentials.anonymous();
        try {
            const user = await app.logIn(credentials);
            const results = await user.functions.getAllBooks();
            setItems(results);
        }
        catch(err) {
            console.error("Failed to log in", err);
        }
    }

    

  useEffect(() => {
    getBooks();
  }, []);

  
  return (
    <div className="movie-list">
        <Swiper
            grabCursor={true}
            spaceBetween={10}
            slidesPerView={'auto'}
        >
            {
                items.map((item, i) => (
                    <SwiperSlide key={i}>
                        <BookCard item={item} category={props.category} />
                    </SwiperSlide>
                ))
            }
        </Swiper>
    </div>
  )
}

BookList.propTypes = {
    category: PropTypes.string.isRequired
}

export default BookList;