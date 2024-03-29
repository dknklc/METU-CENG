import React from 'react';
import './bookcard.scss';
import { Link } from 'react-router-dom';
import Button from '../button/Button';
import { category } from '../../api/tmdbApi';

const BookCard = props => {
    const item  = props.item;
    const link = '/' + category[props.category] + '/' + item._id;
    //const bg = apiConfig.w500Image(item.poster_path || item.backdrop_path);
    const bg = item.img;
    return (
        <Link to={link}>
            <div className="movie-card" style={{backgroundImage: `url(${bg})`}}>
                <Button>
                    <i className="bx bxs-book-open"></i>
                </Button>
            </div>
            <h3>{item.bookName}</h3>
        </Link>
    );
}

export default BookCard;