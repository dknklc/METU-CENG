import React from 'react';
import './footer.scss';
import { Link } from 'react-router-dom';
import bg from '../../assets/books/book4.jpg';
import book from '../../assets/book.png';

const Footer = () => {
    return (
        <div className="footer" style={{backgroundImage: `url(${bg})`}}>
            <div className="footer__content container">
                <div className="footer__content__logo">
                    <div className="logo">
                        <img src={book} alt="" />
                        <Link to="/">Book Tracer</Link>
                    </div>
                </div>
                <div className="footer__content__menus">
                    <div className="footer__content__menu">
                      <Link to="/">About us</Link>
                    </div>
                    <div className="footer__content__menu">
                      <Link to="/">Contact me</Link>
                    </div>
                    <div className="footer__content__menu">
                      <Link to="/">Privacy policy</Link>
                    </div>
                </div>
            </div>
        </div>
    );
}

export default Footer;