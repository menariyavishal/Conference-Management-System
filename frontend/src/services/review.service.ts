import api from './api';
import type { ApiResponse, PaginatedResponse, Review } from '../types/api';

const reviewService = {
  list: async (params?: any): Promise<PaginatedResponse<Review>> => {
    return api.get('/reviews', { params });
  },

  getById: async (id: string): Promise<ApiResponse<Review>> => {
    return api.get(`/reviews/${id}`);
  },

  submit: async (data: any): Promise<ApiResponse<Review>> => {
    return api.post('/reviews', data);
  },

  assign: async (data: { submissionId: string; reviewerId: string }): Promise<ApiResponse<Review>> => {
    return api.post('/reviews/assign', data);
  },

  getStatistics: async (): Promise<ApiResponse<any>> => {
    return api.get('/reviews/statistics');
  }
};

export default reviewService;
