import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { Users, FileText, Settings, Activity, TrendingUp, BarChart3, Loader } from 'lucide-react';
import { useAuth } from '../../context/AuthContext';
import reviewService from '../../services/review.service';

const OrganizerDashboard: React.FC = () => {
  const { user } = useAuth();
  const [stats, setStats] = useState({ users: 0, submissions: 0, reviews: 0, completed: 0 });
  const [isLoading, setIsLoading] = useState(true);
  
  useEffect(() => {
    const fetchData = async () => {
      try {
        setIsLoading(true);
        const response = await reviewService.getStatistics();
        if (response.success) {
          // Assuming backend returns an object with these counts
          // If not, we might need to fallback to dummy data for missing fields
          setStats({
            users: response.data.totalUsers || 0,
            submissions: response.data.totalSubmissions || 0,
            reviews: response.data.totalReviews || 0,
            completed: response.data.completedReviews || 0
          });
        }
      } catch (error) {
        console.error("Error fetching statistics:", error);
      } finally {
        setIsLoading(false);
      }
    };

    fetchData();
  }, []);

  const containerVariants: any = {
    hidden: { opacity: 0 },
    show: { opacity: 1, transition: { staggerChildren: 0.1 } }
  };
  
  const itemVariants: any = {
    hidden: { opacity: 0, y: 20 },
    show: { opacity: 1, y: 0, transition: { type: "spring", stiffness: 300, damping: 24 } }
  };

  return (
    <motion.div variants={containerVariants} initial="hidden" animate="show" className="space-y-8">
      {/* Banner */}
      <motion.div variants={itemVariants} className="relative rounded-3xl overflow-hidden shadow-2xl">
        <div className="absolute inset-0">
          <img 
            src="https://images.unsplash.com/photo-1551818255-e6e10975bc17?auto=format&fit=crop&w=1200&q=80" 
            alt="Data Analytics" 
            className="w-full h-full object-cover opacity-30 mix-blend-overlay"
          />
          <div className="absolute inset-0 bg-gradient-to-r from-purple-900/90 to-[#0a0f1e]/90" />
        </div>
        <div className="relative px-8 py-12 sm:px-12 sm:py-16 flex flex-col md:flex-row justify-between items-start md:items-center">
          <div>
            <h1 className="text-3xl sm:text-4xl font-extrabold tracking-tight text-white mb-2">
              Organizer Command Center
            </h1>
            <p className="text-lg text-purple-200 max-w-2xl font-medium">
              Welcome back, {user?.fullName}. Monitor conference metrics and manage users and submissions.
            </p>
          </div>
          <div className="mt-6 md:mt-0">
             <motion.button 
              whileHover={{ scale: 1.05 }}
              whileTap={{ scale: 0.95 }}
              className="inline-flex items-center px-6 py-3 border border-transparent text-base font-bold rounded-xl shadow-lg shadow-purple-500/30 text-white bg-purple-600 hover:bg-purple-500 transition-colors"
            >
              <Settings className="w-5 h-5 mr-2" />
              Conference Settings
            </motion.button>
          </div>
        </div>
      </motion.div>

      {/* Stats */}
      <div className="grid grid-cols-1 gap-6 sm:grid-cols-2 lg:grid-cols-4">
        {isLoading ? (
          <div className="col-span-full p-12 bg-white/5 rounded-2xl flex items-center justify-center">
            <Loader className="w-8 h-8 text-purple-500 animate-spin mr-3" />
            <p className="text-slate-400">Loading metrics...</p>
          </div>
        ) : (
          [
            { label: 'Total Users', value: stats.users, icon: Users, color: 'text-blue-400', bg: 'bg-blue-400/10' },
            { label: 'Submissions', value: stats.submissions, icon: FileText, color: 'text-purple-400', bg: 'bg-purple-400/10' },
            { label: 'Active Reviews', value: stats.reviews, icon: Activity, color: 'text-amber-400', bg: 'bg-amber-400/10' },
            { label: 'Completed', value: stats.completed, icon: TrendingUp, color: 'text-emerald-400', bg: 'bg-emerald-400/10' }
          ].map((stat, idx) => (
            <motion.div key={idx} variants={itemVariants} whileHover={{ y: -5 }} className="bg-[#0f172a]/80 backdrop-blur-xl border border-white/5 rounded-2xl p-6 shadow-xl transition-all">
              <div className="flex items-center justify-between">
                <div>
                  <p className="text-sm font-medium text-slate-400 uppercase tracking-wider">{stat.label}</p>
                  <p className="mt-2 text-4xl font-extrabold text-white">{stat.value}</p>
                </div>
                <div className={`p-4 rounded-xl ${stat.bg}`}>
                  <stat.icon className={`w-8 h-8 ${stat.color}`} />
                </div>
              </div>
            </motion.div>
          ))
        )}
      </div>

      {/* Recent Activity */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
        <motion.div variants={itemVariants} className="lg:col-span-2 bg-[#0f172a]/80 backdrop-blur-xl border border-white/5 rounded-3xl shadow-xl overflow-hidden">
          <div className="px-6 py-5 border-b border-white/10 flex justify-between items-center">
            <h3 className="text-xl font-bold text-white">System Activity</h3>
            <BarChart3 className="w-5 h-5 text-slate-400" />
          </div>
          <div className="p-8 text-center h-64 flex flex-col justify-center items-center">
            <Activity className="w-12 h-12 text-slate-600 mb-4" />
            <p className="text-slate-400">Activity chart will render here.</p>
          </div>
        </motion.div>
        
        <motion.div variants={itemVariants} className="bg-[#0f172a]/80 backdrop-blur-xl border border-white/5 rounded-3xl shadow-xl overflow-hidden">
          <div className="px-6 py-5 border-b border-white/10">
            <h3 className="text-xl font-bold text-white">Quick Actions</h3>
          </div>
          <div className="p-4 space-y-2">
            {['Export Submissions', 'Manage Tracks', 'Email Announcements', 'View Audit Logs'].map((action, idx) => (
              <button key={idx} className="w-full text-left px-4 py-3 rounded-xl bg-white/5 hover:bg-white/10 text-slate-300 hover:text-white transition-colors text-sm font-medium border border-white/5 hover:border-white/10">
                {action}
              </button>
            ))}
          </div>
        </motion.div>
      </div>
    </motion.div>
  );
};

export default OrganizerDashboard;
