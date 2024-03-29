import React from 'react';
import { useParams } from 'react-router-dom';
import PageHeader from '../components/page-header/PageHeader';
import { category as cate } from '../api/tmdbApi';
import BookGrid from '../components/book-grid/BookGrid';

const Catalog = () => {

    const { category } = useParams();
    console.log(category);
    return (
        <>
            <PageHeader>
                {category === cate.books ? 'Books' : 'Users'}
            </PageHeader>
            <div className="container">
                <div className="section mb-3">
                    <BookGrid category={category}/>
                </div>
            </div>
        </>
    );
}

export default Catalog;